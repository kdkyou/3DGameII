
// CPU→GPUに送り込む全Shaderの共有データ
#include "inc_KdShaderCore.hlsli"

// 3D描画が終わったお青の画面に映る直前の状態
Texture2D g_tex : register(t0);

// サンプラーステート(テクスチャから色を取ってくスポイト)
#define g_ss g_ss_linear_clamp


//ポストプロセスの頂点シェーダーから送り込まれるデータ

struct PSInput
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD0;
};

// モノクロ変換
float4 GrayScale(PSInput In):SV_Target0
{
    // テクスチャから色を取ってくる
    float4 outColor = g_tex.Sample(g_ss,In.UV);
    // モノクロ変換
    float gray = outColor.r*0.299f + outColor.g * 0.587f + outColor.b *0.114f;
    
    outColor.rgb = gray;
    
    return outColor;
};

// セピア変換
// グレースケールした後に経年変化の表現を加える
float4 Sepia(PSInput In):SV_Target0
{
    float4 outColor = g_tex.Sample(g_ss_aniso_clamp, In.UV);
    float gray = outColor.x;
    
    // セピア変換
    outColor.r = gray;
    outColor.g = gray * (74.0f / 107.0f);
    outColor.b = gray * (43.0f / 107.0f);
    
    return outColor;
    
}

// 二階調表現（白か黒かの二択）
// 白黒変換された後前提
float g_twThreshold;
float4 TwoGradiation(PSInput In):SV_Target0
{
    float4 outColor = g_tex.Sample(g_ss, In.UV);
    float gray = outColor.r;
    if (gray > g_twThreshold)
    {
        outColor.rgb = 1.0f;
    }
    else
    {
        outColor.rgb = 0.0f;
    }

    return outColor;
}

// ビネット表現
float g_vignetteIntenPow;
float g_vignetteMaskPow;
Texture2D g_vignetteMask : register(t1);
float4 vignette(PSInput In):SV_Target0
{
    
    float4 outColor = g_tex.Sample(g_ss, In.UV);
    
    float4 maskColor = g_vignetteMask.Sample(g_ss, In.UV);
    
    // ビネットの中心地
    float2 vCenter = float2(0.5f,0.5f);
    
    // 中心点からどれだけ離れているか
    float len = length(vCenter - In.UV);
    len = saturate(len);
    
    float intensity = 1 - len;
    
    intensity = pow(intensity, g_vignetteIntenPow);
    
    intensity += maskColor.r * g_vignetteMaskPow; //マスクの模様を足す
    intensity = saturate(intensity);
    
    outColor.rgb *= intensity;
    
    return outColor;
    
}