#include "inc_KdShaderCore.hlsli"

#include "inc_PBRCommon.hlsli"

//================================
//
// �}�e���A��
//
//================================
cbuffer cbPerMaterial : register(b0)
{
    float4 g_BaseColor;
    float4 g_EmissiveColor;
    float g_Metallic;
    float g_Smoothness;
};


// �e�N�X�`��
Texture2D g_baseMap : register(t0); // �x�[�X�J���[�e�N�X�`��
Texture2D g_normalMap : register(t1); // �@���}�b�v
Texture2D g_metallicSmoothnesMap : register(t2); // ���炩���������}�b�v
Texture2D g_emissiveMap : register(t3); // ���ːF�}�b�v

// �T���v��
//SamplerState g_ss : register(s0);
//SamplerComparisonState g_ss_comparison : register(s15);



//================================

// ����
// �E�V�F�[�_�[�͊֐����œo�^�����Pass�����肵�܂��B�ڍׂ�KdLitShader.cpp�Q�ƁB
// �@��)VS�APS�c Pass[0]�ɓo�^
// �@��)ShadowCasterVS�AShadowCasterPS�c Pass[1]�ɓo�^


//========================================================
// Pass:0
// �ʏ�`��
//
//========================================================

// ���_�V�F�[�_����o�͂���f�[�^
struct VSOutput
{
    float4 Pos : SV_Position; // �ˉe���W
    float2 UV : TEXCOORD0; // UV���W
    float3 wT : TEXCOORD1; // ���[���h�ڐ�
    float3 wB : TEXCOORD2; // ���[���h�]�@��
    float3 wN : TEXCOORD3; // ���[���h�@��
//    float4 Color : TEXCOORD4;   // �F
    float3 wPos : TEXCOORD5; // ���[���h3D���W
};

// �s�N�Z���V�F�[�_�[����o�͂���f�[�^
struct PSOutput
{
    float4 color : SV_Target0; // �F
    float4 normal : SV_Target1; // �@���x�N�g��
};


//-------------------------------
// ���_�V�F�[�_
//================================

// ���_�V�F�[�_
VSOutput VS(  float4 pos : POSITION,  // ���_���W
                float2 uv : TEXCOORD0,  // �e�N�X�`��UV���W
                float3 tangent : TANGENT,// �ڐ��x�N�g��
                float3 normal : NORMAL  // �@���x�N�g��
//                float4 color : COLOR
){
    VSOutput Out;

    // ���W�ϊ�
    Out.Pos = mul(pos, g_mW);       // ���[�J�����W�n -> ���[���h���W�n�֕ϊ�
    Out.wPos = Out.Pos.xyz;
    Out.Pos = mul(Out.Pos, g_mV);   // ���[���h���W�n -> �r���[���W�n�֕ϊ�
    Out.Pos = mul(Out.Pos, g_mP);   // �r���[���W�n -> �ˉe���W�n�֕ϊ�

    // �@��
    Out.wT = normalize(mul(tangent, (float3x3)g_mW));
    Out.wN = normalize(mul(normal, (float3x3)g_mW));
    Out.wB = normalize(cross(Out.wN, Out.wT));
    
    // UV���W
    Out.UV = uv;
    
    // �o��
    return Out;
}



//-------------------------------
// �s�N�Z���V�F�[�_
//-------------------------------

// �s�N�Z���V�F�[�_
PSOutput PS(VSOutput In) : SV_Target0
{
    PSOutput Out = (PSOutput) 0; //
    
    // �J�����ւ̕���
    float3 vCam = g_CamPos - In.wPos;
    float camDist = length(vCam); // �J���� - �s�N�Z������
    vCam = normalize(vCam);
    
   
    // �@��
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
    // �ގ��F
    //------------------------------------------

	// Specular coefficiant - ������Œ蔽�˗�
    static const float3 kSpecularCoefficient = float3(0.04, 0.04, 0.04);

    float4 mr = g_metallicSmoothnesMap.Sample(g_ss_aniso_wrap, In.UV);
    // ������
    float metallic = mr.b * g_Metallic;
    // �e��
    float roughness = 1.0 - mr.g * g_Smoothness;
	// ���t�l�X��2�悵���ق����A���o�I�Ƀ��j�A�Ɍ�����炵��
    const float roughness2 = roughness * roughness;

    // �ގ��̐F
    float4 baseColor = g_baseMap.Sample(g_ss_aniso_wrap, In.UV) * g_BaseColor /*In.Color*/;

    // �ގ��̊g�U�F�@������قǍގ��̐F�ɂȂ�A�����قǊg�U�F�͖����Ȃ�
    const float3 baseDiffuse = lerp(baseColor.rgb * (1.0 - kSpecularCoefficient), float3(0, 0, 0), metallic);
    // �ގ��̔��ːF�@������قǌ��̐F�����̂܂ܔ��˂��A�����قǍގ��̐F�����
    const float3 baseSpecular = lerp(kSpecularCoefficient, baseColor.rgb, metallic);

    // �ŏI�I�ȐF
    float3 color = 0;
    
    //------------------
    // ���s��
    //------------------
    {
		// Half vector
        const float3 H = normalize(-g_DL_Dir + vCam);

        const float NdotL = saturate(dot(wN, -g_DL_Dir));
        const float LdotH = saturate(dot(-g_DL_Dir, H));
        const float NdotH = saturate(dot(wN, H));

        // �e
        float shadow = 1;
        if (g_DL_ShadowPower > 0)
        {
            float4 liPos = mul(float4(In.wPos, 1), g_DL_mLightVP);
            liPos.xyz /= liPos.w;

			// �[�x�}�b�v�͈͓̔��H
			if (abs(liPos.x) <= 1 && abs(liPos.y) <= 1 && liPos.z <= 1)
            {
				// �ˉe���W -> UV���W�֕ϊ�
                float2 uv = liPos.xy * float2(1, -1) * 0.5 + 0.5;

				// ���C�g�J��������̋������A�o�C�A�X�����炷(�V���h�E�A�N�l�΍�)
                float bias = g_DL_DirLightShadowBias * tan(acos(NdotL));
                bias = clamp(bias, 0, 0.01); // �V���h�E�A�N�l�΍��p�o�C�A�X
                float z = liPos.z - bias; // �V���h�E�A�N�l�΍�


                // ��ʂ̃T�C�Y
                float2 pxSize;
                g_dirLightShadowMap.GetDimensions(pxSize.x, pxSize.y);
                
				// �|���\���T���v�����O�ŁA���ӂ̃s�N�Z�����e����
                shadow = 0;
                for (int i = 0; i < 4; i++)
                {
                    shadow += g_dirLightShadowMap.SampleCmpLevelZero(g_ss_comparison, uv + g_poissonDisk[i] / pxSize.xy, z);
                }
                shadow *= 0.25;
            }
        }
        
		// �g�U��
        float3 diffuse = Diffuse_Burley(baseDiffuse, NdotL, NdotV, LdotH, roughness);
		// ���ˌ�
        float3 specular = Specular_BRDF(roughness2, baseSpecular, NdotV, NdotL, LdotH, NdotH);

		// �������Z
        color += NdotL * g_DL_Color * (diffuse + specular) * shadow;
        
    }
    
    //------------------
    // ����
    //------------------
    color += g_AmbientLight * baseColor.rgb * baseColor.a;
    
    //------------------
    // �G�~�b�V�u
    //------------------
    color += g_emissiveMap.Sample(g_ss_aniso_wrap, In.UV).rgb * g_EmissiveColor.rgb;
    
	//------------------
	// IBL
	//------------------
	// �g�U��
    float3 envDiff = g_IBLTex.SampleLevel(g_ss_aniso_wrap, wN, 8).rgb; // �g�U�\�����邽�߁A����𑜓x�̉摜���g�p����
	color += envDiff * baseDiffuse.rgb / 3.141592;

	// ���ˌ�
	float3 vRef = reflect(-vCam, wN);
    float3 envSpec = g_IBLTex.SampleLevel(g_ss_aniso_wrap, vRef, roughness * 8).rgb; // �e���قǒ�𑜓x�̉摜���g�p����
	color += envSpec * baseSpecular;

	//------------------------------------------
	// �����t�H�O
	//------------------------------------------
    if (g_DistanceFogDensity > 0)
    {
        float d = camDist * g_DistanceFogDensity;
        
		// �w���t�H�O 1(�߂�)�`0(����)
        float f = saturate(1.0 / exp(d * d));
		// �K�p
        color = lerp(g_DistanceFogColor, color, f);
    }
    
    //------------------------------------------
    // �o��
    //------------------------------------------
    Out.color = float4(color, baseColor.a);
    Out.normal = float4(wN, 1);
    return Out;

}

//========================================================
// Pass:1
// �V���h�E�}�b�v�����`��
//
//========================================================

// ���_�V�F�[�_����o�͂���f�[�^
struct ShadowCasterVSOutput
{
    float4 Pos : SV_Position; // �ˉe���W
    float2 UV : TEXCOORD0; // UV���W
    float4 wvpPos : TEXCOORD1;
};

// ���_�V�F�[�_
ShadowCasterVSOutput ShadowCasterVS(float4 pos : POSITION, // ���_���W
                        float2 uv : TEXCOORD0 // �e�N�X�`��UV���W
//                        float3 tangent : TANGENT, // �ڐ��x�N�g��
//                        float3 normal : NORMAL // �@���x�N�g��
//                float4 color : COLOR
)
{
    ShadowCasterVSOutput Out;

    // ���W�ϊ�
    Out.Pos = mul(pos, g_mW); // ���[�J�����W�n -> ���[���h���W�n�֕ϊ�
    Out.Pos = mul(Out.Pos, g_mV); // ���[���h���W�n -> �r���[���W�n�֕ϊ�
    Out.Pos = mul(Out.Pos, g_mP); // �r���[���W�n -> �ˉe���W�n�֕ϊ�

    Out.wvpPos = Out.Pos;
    
    // UV���W
    Out.UV = uv;
    
    // �o��
    return Out;
}

// �s�N�Z���V�F�[�_
float4 ShadowCasterPS(ShadowCasterVSOutput In) : SV_Target0
{
    return In.wvpPos.z / In.wvpPos.w;
}