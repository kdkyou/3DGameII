#include "inc_KdShaderCore.hlsli"

// 
cbuffer cbMaterial : register(b0)
{
    int g_SampleCount;        // �T���v����
    float g_Radius;
    float g_Sigma;
    float g_Beta;
    float g_Epsilon;
};

struct PSInput
{
    float4 Pos : SV_Position; // �ˉe���W
    float2 UV : TEXCOORD0; // UV���W
};

Texture2D g_inputTex : register(t0);

//SamplerState g_ss : g_linear_clamp;
#define g_ss g_ss_linear_clamp

//============================================
//
// DownSampling2x2
//
//============================================

// �萔�o�b�t�@
cbuffer cbMaterial : register(b0)
{
    float4 g_sampleOffsets[4];
};

float4 DownSampling2x2PS(PSInput In) : SV_Target0
{
    float3 color = 0;

//    color = g_inputTex.Sample(g_ss_clamp, In.UV).rgb;
    
    color += g_inputTex.Sample(g_ss, In.UV + g_sampleOffsets[0].xy).rgb;
    color += g_inputTex.Sample(g_ss, In.UV + g_sampleOffsets[1].xy).rgb;
    color += g_inputTex.Sample(g_ss, In.UV + g_sampleOffsets[2].xy).rgb;
    color += g_inputTex.Sample(g_ss, In.UV + g_sampleOffsets[3].xy).rgb;
    color *= 0.25;
    
    return float4(color, 1);
}

//============================================
//
// ���P�x���o
//
//============================================

// �萔�o�b�t�@
cbuffer cbMaterial : register(b0)
{
    float g_BrightThreshold;
};

float4 BrightFilteringPS(PSInput In) : SV_Target0
{
    float4 color = g_inputTex.Sample(g_ss, In.UV);

    color.rgb = max(0, color.rgb - g_BrightThreshold);
    
    return color;
}

//============================================
//
// Blur
//
//============================================

// �萔�o�b�t�@
cbuffer cbMaterial : register(b0)
{
//    float4 g_offset[31]; // xy:�I�t�Z�b�g���W z:�d��
    int g_sampleCount;
    float2 g_direction;
    float g_dispersion;
};

// �K�E�X�֐�
float GetGaussianWeight(float distance, float dispersion)
{
    return exp(-(distance * distance) / (2 * dispersion * dispersion));
}

float4 BlurPS(PSInput In) : SV_Target0
{
    float totalW = 0;
    float w[100];
    
    // �T���v�����Ԃ�A�K�E�X�֐��ŏd�݂����߂�
    for (int j = 0; j < g_sampleCount; j++)
    {
        int offset = (j - g_sampleCount / 2);
        
        w[j] = GetGaussianWeight(offset, g_dispersion); //�E�F�C�g���v�Z
        totalW += w[j];
    }
    // �d�݂𐳋K��
    for (j = 0; j < g_sampleCount; j++)
    {
        // �E�F�C�g���K��
        w[j] /= totalW;
    }

    // g_direction�̕������ӂ̐F���A�d�݂��g�p���č������Ă���(�ڂ���)
    float4 color = 0;
    for (j = 0; j < g_sampleCount; j++)
    {
        int offset = (j - g_sampleCount / 2);
        float2 addUV = g_direction.xy * offset;

        color += g_inputTex.Sample(g_ss, In.UV + addUV) * w[j];
    }
    
    return color;
}
