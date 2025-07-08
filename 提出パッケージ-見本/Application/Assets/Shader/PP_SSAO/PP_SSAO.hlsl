#include "inc_KdShaderCore.hlsli"

// 
cbuffer cbMaterial : register(b0)
{
    int g_SampleCount;        // サンプル数
    float g_Radius;
    float g_Sigma;
    float g_Beta;
    float g_Epsilon;
};

struct PSInput
{
    float4 Pos : SV_Position; // 射影座標
    float2 UV : TEXCOORD0; // UV座標
};

Texture2D g_inputTex : register(t0);

//Texture2D g_colorTex : register(t100);
//Texture2D g_normalTex : register(t101);
//Texture2D g_depthTex : register(t102);

#define g_ss g_ss_linear_clamp

//============================================
//
// Alchemy SSAO
//
//============================================

// 0.0f ～ 1.0fの乱数発生
float GetRandomNumber(float2 texCoord, int Seed)
{
    return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
} 

float2 GetRamdomHemisphereXY(float2 uv, int seed)
{
    const float r1 = GetRandomNumber(uv, seed);
    const float r2 = frac(r1 * 2558.54f);
    const float r3 = frac(r2 * 1924.19f);
    const float r = pow(r1, 1.0f / 3.0f) * sqrt(1.0f - r3 * r3);
    const float theta = 2 * 3.14159265 * r2;
    float s, c;
    sincos(theta, s, c);
    return float2(c, s) * r;
}

float4 ConvertUVtoWorld(in float2 uv, in float depth)
{
    float4 pos3D = float4((uv * 2 - 1) * float2(1, -1), depth, 1);
    pos3D = mul(pos3D, g_mInvVP);
    pos3D.xyz /= pos3D.w;
    return pos3D;
}

float4 SSAOPS(PSInput In) : SV_Target0
{
    float depth = g_depthTex.Sample(g_ss, In.UV).r;

    float3 normal = normalize(g_normalTex.Sample(g_ss, In.UV).xyz);

    // 深度 -> 3D座標
    float4 pos3D = ConvertUVtoWorld(In.UV, depth);

    float radiusScaled = g_Radius * pos3D.w;

    // 周囲の遮蔽状況からAOを計算
    float ao = 0;
    for (int i = 0; i < g_SampleCount; i++)
    {
        const float2 sampleUV = In.UV + GetRamdomHemisphereXY(In.UV, i) * radiusScaled;

        float d2 = g_depthTex.Sample(g_ss, sampleUV).r;
        // 3D座標へ変換
        float4 pos3DTarget = ConvertUVtoWorld(sampleUV, d2);

        // ヒット位置とピクセル位置の方向
        float3 v = pos3DTarget.xyz - pos3D.xyz;

        // AO計算(Alchemy SSAO)
        ao += max(0, dot(v, normal) + d2*g_Beta) / (dot(v, v) + g_Epsilon);
    }
    ao = max(0, 1.0 - ((2 * g_Sigma) / g_SampleCount) * ao);

    return float4(ao.xxx, 1);
}

//============================================
//
// BilateralBlur
//
//============================================

// 定数バッファ
cbuffer cbMaterial : register(b0)
{
//    float4 g_offset[31]; // xy:オフセット座標 z:重み
    int g_bb_SampleCount;
    float2 g_direction;
    float g_dispersion;
    float g_lumin;
};

float GetGaussianWeight(float distance, float dispersion)
{
    return exp(-(distance * distance) / (2 * dispersion * dispersion));// / dispersion;
}

float4 BilateralBlurPS(PSInput In) : SV_Target0
{
    // 輝度計算用定数
    static const float3 kPerception = float3(0.299, 0.587, 0.114);

	// 参考：https://imagingsolution.net/imaging/bilateralfilter/

	// 現在のピクセルの輝度
    float3 baseColor = g_inputTex.Sample(g_ss, In.UV).rgb;
    float baseLumin = dot(kPerception, baseColor); // 輝度算出

	// サンプリング先の輝度差から重みを乗算
    /*
    float w[31];
    float total = 0;
    for (int i = 0; i < 31; i++)
    {
        float3 targetColor = g_inputTex.Sample(g_ss_clamp, In.UV + g_offset[i].xy).rgb;
        float targetLumin = dot(Perception, targetColor); // 輝度算出

		// 距離差 * 輝度差
        float differenceLumin = (baseLumin - targetLumin);
        w[i] = g_offset[i].z * exp(-(differenceLumin * differenceLumin) / (2 * g_lumin * g_lumin));
        total += w[i];
    }
    */
    
//    float g_dispersion = 1;
    
    float totalW = 0;
    float w[100];
    /*
    for (int j = 0; j < g_samplingTexelAmount; j++)
    {
        int x = (j - g_samplingTexelAmount / 2);
        float2 offset = g_direction.xy * x;
        
        float3 targetColor = g_inputTex.Sample(g_ss_clamp, In.UV + offset).rgb;
        
        // 輝度算出
        float targetLumin = dot(Perception, targetColor);
		// 距離差 * 輝度差
        float differenceLumin = (baseLumin - targetLumin);
//        weight *= exp(-(differenceLumin * differenceLumin) / (2 * g_lumin * g_lumin));
        
        float weight = GetGaussianWeight(x, g_dispersion); //ウェイトを計算
        w[j] = weight; // * exp(-(differenceLumin * differenceLumin) / (2 * g_lumin * g_lumin));
        totalW += w[j];
    }
    */
    
    // 
    for (int j = 0; j < g_bb_SampleCount; j++)
    {
        int x = (j - g_bb_SampleCount / 2);
        float2 offset = g_direction.xy * x;
        
        w[j] = GetGaussianWeight(x, g_dispersion); //ウェイトを計算
        totalW += w[j];
    }

    float totalW2 = 0;
    for (j = 0; j < g_bb_SampleCount; j++)
    {
        // ウェイト正規化
        w[j] /= totalW;
        
        int x = (j - g_bb_SampleCount / 2);
        float2 offset = g_direction.xy * x;

        // 色取得
        float3 targetColor = g_inputTex.Sample(g_ss, In.UV + offset).rgb;
        // 輝度算出
        float targetLumin = dot(kPerception, targetColor);

		// 距離差 * 輝度差
        float differenceLumin = (baseLumin - targetLumin);

        w[j] = w[j] * exp(-(differenceLumin * differenceLumin) / (2 * g_lumin * g_lumin));

        totalW2 += w[j];
        
    }

    
    float3 color = 0;
    for (j = 0; j < g_bb_SampleCount; j++)
    {
        int x = (j - g_bb_SampleCount / 2);
        float2 offset = g_direction.xy * x;

        color += g_inputTex.Sample(g_ss, In.UV + offset).rgb * (w[j] / totalW2);
    }
    
//    color /= totalW;

    return float4(color, 1);
}
