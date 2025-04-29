#include "inc_KdShaderCore.hlsli"

#include "inc_PBRCommon.hlsli"

//================================
//
// マテリアル
//
//================================
cbuffer cbPerMaterial : register(b0)
{
    float4 g_BaseColor;
    float4 g_EmissiveColor;
    float g_Metallic;
    float g_Smoothness;
};


// テクスチャ
Texture2D g_baseMap : register(t0); // ベースカラーテクスチャ
Texture2D g_normalMap : register(t1); // 法線マップ
Texture2D g_metallicSmoothnesMap : register(t2); // 滑らかさ＆金属マップ
Texture2D g_emissiveMap : register(t3); // 放射色マップ

// サンプラ
//SamplerState g_ss : register(s0);
//SamplerComparisonState g_ss_comparison : register(s15);



//================================

// 説明
// ・シェーダーは関数名で登録されるPassが決定します。詳細はKdLitShader.cpp参照。
// 　例)VS、PS… Pass[0]に登録
// 　例)ShadowCasterVS、ShadowCasterPS… Pass[1]に登録


//========================================================
// Pass:0
// 通常描画
//
//========================================================

// 頂点シェーダから出力するデータ
struct VSOutput
{
    float4 Pos : SV_Position; // 射影座標
    float2 UV : TEXCOORD0; // UV座標
    float3 wT : TEXCOORD1; // ワールド接線
    float3 wB : TEXCOORD2; // ワールド従法線
    float3 wN : TEXCOORD3; // ワールド法線
//    float4 Color : TEXCOORD4;   // 色
    float3 wPos : TEXCOORD5; // ワールド3D座標
};

// ピクセルシェーダーから出力するデータ
struct PSOutput
{
    float4 color : SV_Target0; // 色
    float4 normal : SV_Target1; // 法線ベクトル
};


//-------------------------------
// 頂点シェーダ
//================================

// 頂点シェーダ
VSOutput VS(  float4 pos : POSITION,  // 頂点座標
                float2 uv : TEXCOORD0,  // テクスチャUV座標
                float3 tangent : TANGENT,// 接線ベクトル
                float3 normal : NORMAL  // 法線ベクトル
//                float4 color : COLOR
){
    VSOutput Out;

    // 座標変換
    Out.Pos = mul(pos, g_mW);       // ローカル座標系 -> ワールド座標系へ変換
    Out.wPos = Out.Pos.xyz;
    Out.Pos = mul(Out.Pos, g_mV);   // ワールド座標系 -> ビュー座標系へ変換
    Out.Pos = mul(Out.Pos, g_mP);   // ビュー座標系 -> 射影座標系へ変換

    // 法線
    Out.wT = normalize(mul(tangent, (float3x3)g_mW));
    Out.wN = normalize(mul(normal, (float3x3)g_mW));
    Out.wB = normalize(cross(Out.wN, Out.wT));
    
    // UV座標
    Out.UV = uv;
    
    // 出力
    return Out;
}



//-------------------------------
// ピクセルシェーダ
//-------------------------------

// ピクセルシェーダ
PSOutput PS(VSOutput In) : SV_Target0
{
    PSOutput Out = (PSOutput) 0; //
    
    // カメラへの方向
    float3 vCam = g_CamPos - In.wPos;
    float camDist = length(vCam); // カメラ - ピクセル距離
    vCam = normalize(vCam);
    
   
    // 法線
    float3x3 mTBN =
    {
        normalize(In.wT),
        normalize(In.wB),
        normalize(In.wN)
    };
    
    float3 wN = g_normalMap.Sample(g_ss_aniso_wrap, In.UV).rgb;
    wN = wN * 2.0 - 1.0;
    wN = mul(wN, mTBN);

    float NdotV = saturate(dot(wN, vCam));
    
    //------------------------------------------
    // 材質色
    //------------------------------------------

	// Specular coefficiant - 非金属固定反射率
    static const float3 kSpecularCoefficient = float3(0.04, 0.04, 0.04);

    float4 mr = g_metallicSmoothnesMap.Sample(g_ss_aniso_wrap, In.UV);
    // 金属性
    float metallic = mr.b * g_Metallic;
    // 粗さ
    float roughness = 1.0 - mr.g * g_Smoothness;
	// ラフネスを2乗したほうが、感覚的にリニアに見えるらしい
    const float roughness2 = roughness * roughness;

    // 材質の色
    float4 baseColor = g_baseMap.Sample(g_ss_aniso_wrap, In.UV) * g_BaseColor /*In.Color*/;

    // 材質の拡散色　非金属ほど材質の色になり、金属ほど拡散色は無くなる
    const float3 baseDiffuse = lerp(baseColor.rgb * (1.0 - kSpecularCoefficient), float3(0, 0, 0), metallic);
    // 材質の反射色　非金属ほど光の色をそのまま反射し、金属ほど材質の色が乗る
    const float3 baseSpecular = lerp(kSpecularCoefficient, baseColor.rgb, metallic);

    // 最終的な色
    float3 color = 0;
    
    //------------------
    // 平行光
    //------------------
    {
		// Half vector
        const float3 H = normalize(-g_DL_Dir + vCam);

        const float NdotL = saturate(dot(wN, -g_DL_Dir));
        const float LdotH = saturate(dot(-g_DL_Dir, H));
        const float NdotH = saturate(dot(wN, H));

        // 影
        float shadow = 1;
        if (g_DL_ShadowPower > 0)
        {
            float4 liPos = mul(float4(In.wPos, 1), g_DL_mLightVP);
            liPos.xyz /= liPos.w;

			// 深度マップの範囲内？
			if (abs(liPos.x) <= 1 && abs(liPos.y) <= 1 && liPos.z <= 1)
            {
				// 射影座標 -> UV座標へ変換
                float2 uv = liPos.xy * float2(1, -1) * 0.5 + 0.5;

				// ライトカメラからの距離を、バイアス分減らす(シャドウアクネ対策)
                float bias = g_DL_DirLightShadowBias * tan(acos(NdotL));
                bias = clamp(bias, 0, 0.01); // シャドウアクネ対策用バイアス
                float z = liPos.z - bias; // シャドウアクネ対策


                // 画面のサイズ
                float2 pxSize;
                g_dirLightShadowMap.GetDimensions(pxSize.x, pxSize.y);
                
				// ポワソンサンプリングで、周辺のピクセルを影判定
                shadow = 0;
                for (int i = 0; i < 4; i++)
                {
                    shadow += g_dirLightShadowMap.SampleCmpLevelZero(g_ss_comparison, uv + g_poissonDisk[i] / pxSize.xy, z);
                }
                shadow *= 0.25;
            }
        }
        
		// 拡散光
        float3 diffuse = Diffuse_Burley(baseDiffuse, NdotL, NdotV, LdotH, roughness);
		// 反射光
        float3 specular = Specular_BRDF(roughness2, baseSpecular, NdotV, NdotL, LdotH, NdotH);

		// 光を加算
        color += NdotL * g_DL_Color * (diffuse + specular) * shadow;
        
    }
    
    //------------------
    // 環境光
    //------------------
    color += g_AmbientLight * baseColor.rgb * baseColor.a;
    
    //------------------
    // エミッシブ
    //------------------
    color += g_emissiveMap.Sample(g_ss_aniso_wrap, In.UV).rgb * g_EmissiveColor.rgb;
    
	//------------------
	// IBL
	//------------------
	// 拡散光
    float3 envDiff = g_IBLTex.SampleLevel(g_ss_aniso_wrap, wN, 8).rgb; // 拡散表現するため、超低解像度の画像を使用する
	color += envDiff * baseDiffuse.rgb / 3.141592;

	// 反射光
	float3 vRef = reflect(-vCam, wN);
    float3 envSpec = g_IBLTex.SampleLevel(g_ss_aniso_wrap, vRef, roughness * 8).rgb; // 粗いほど低解像度の画像を使用する
	color += envSpec * baseSpecular;

	//------------------------------------------
	// 距離フォグ
	//------------------------------------------
    if (g_DistanceFogDensity > 0)
    {
        float d = camDist * g_DistanceFogDensity;
        
		// 指数フォグ 1(近い)～0(遠い)
        float f = saturate(1.0 / exp(d * d));
		// 適用
        color = lerp(g_DistanceFogColor, color, f);
    }
    
    //------------------------------------------
    // 出力
    //------------------------------------------
    Out.color = float4(color, baseColor.a);
    Out.normal = float4(wN, 1);
    return Out;

}

//========================================================
// Pass:1
// シャドウマップ生成描画
//
//========================================================

// 頂点シェーダから出力するデータ
struct ShadowCasterVSOutput
{
    float4 Pos : SV_Position; // 射影座標
    float2 UV : TEXCOORD0; // UV座標
    float4 wvpPos : TEXCOORD1;
};

// 頂点シェーダ
ShadowCasterVSOutput ShadowCasterVS(float4 pos : POSITION, // 頂点座標
                        float2 uv : TEXCOORD0 // テクスチャUV座標
//                        float3 tangent : TANGENT, // 接線ベクトル
//                        float3 normal : NORMAL // 法線ベクトル
//                float4 color : COLOR
)
{
    ShadowCasterVSOutput Out;

    // 座標変換
    Out.Pos = mul(pos, g_mW); // ローカル座標系 -> ワールド座標系へ変換
    Out.Pos = mul(Out.Pos, g_mV); // ワールド座標系 -> ビュー座標系へ変換
    Out.Pos = mul(Out.Pos, g_mP); // ビュー座標系 -> 射影座標系へ変換

    Out.wvpPos = Out.Pos;
    
    // UV座標
    Out.UV = uv;
    
    // 出力
    return Out;
}

// ピクセルシェーダ
float4 ShadowCasterPS(ShadowCasterVSOutput In) : SV_Target0
{
    return In.wvpPos.z / In.wvpPos.w;
}