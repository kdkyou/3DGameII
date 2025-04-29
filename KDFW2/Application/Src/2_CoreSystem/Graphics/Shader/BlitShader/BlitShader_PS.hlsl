// テクスチャ
Texture2D g_tex : register(t0); // ベースカラーテクスチャ
// サンプラ
SamplerState g_ss : register(s0);

struct PSInput
{
    float4 Pos : SV_Position; // 射影座標
    float2 UV : TEXCOORD0; // UV座標
};
//================================
//
// ピクセルシェーダ
//
//================================
float4 main(PSInput In) : SV_Target0
{
    return g_tex.Sample(g_ss, In.UV);
}
