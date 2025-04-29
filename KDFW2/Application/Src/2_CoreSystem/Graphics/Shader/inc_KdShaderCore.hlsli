//----------------------------------------
// ���ʃe�N�X�`��
//----------------------------------------

Texture2D g_colorTex : register(t100);
Texture2D g_normalTex : register(t101);
Texture2D g_depthTex : register(t102);

// ���s��ShadowMap
Texture2D g_dirLightShadowMap : register(t103);


// IBL
TextureCube g_IBLTex : register(t110);

//----------------------------------------
// ���ʃT���v��
//----------------------------------------
SamplerState g_ss_aniso_wrap : register(s0);
SamplerState g_ss_aniso_clamp : register(s1);
SamplerState g_ss_linear_wrap : register(s2);
SamplerState g_ss_linear_clamp : register(s3);
SamplerState g_ss_point_wrap : register(s4);
SamplerState g_ss_point_clamp : register(s5);

SamplerComparisonState g_ss_comparison : register(s15);

//----------------------------------------
// �萔�o�b�t�@[0]�F�}�e���A���P��
//----------------------------------------
/*
cbuffer cbPerMaterial : register(b0)
{
};
*/

//----------------------------------------
// �萔�o�b�t�@[1]�F�`��P��
//----------------------------------------
cbuffer cbPerDraw : register(b1)
{
    row_major float4x4 g_mW;    // ���[���h�ϊ��s��
};

//----------------------------------------
// �萔�o�b�t�@[7]�F�J�����P��
//----------------------------------------
cbuffer cbPerCamera : register(b7)
{
    // �J�������
    row_major float4x4 g_mV;    // �r���[�ϊ��s��
    row_major float4x4 g_mInvV; // �r���[�ϊ��s��(�t�s��)
    row_major float4x4 g_mP;    // �ˉe�ϊ��s��
    row_major float4x4 g_mInvP; // �ˉe�ϊ��s��(�t�s��)

    row_major float4x4 g_mVP;       // �r���[*�ˉe�ϊ��s��
    row_major float4x4 g_mInvVP; // �r���[*�ˉe�ϊ��s��(�t�s��)

    float3 g_CamPos;            // �J�������W(World)
};


//----------------------------------------
// �萔�o�b�t�@[8]�F���C�g
//----------------------------------------

// �X�|�b�g���C�g�̃f�[�^
struct SpotLight
{
    float3 Dir; // ���̕���
    float Range; // �͈�
    float3 Color; // �F
    float InnerCorn; // �����̊p�x
    float3 Pos; // ���W
    float OuterCorn; // �O���̊p�x

    float EnableShadow;
    float ShadowBias;
    row_major float4x4 mLightVP;
};

// �|�C���g���C�g�̃f�[�^
struct PointLight
{
    float3 Color; // �F
    float Radius; // ���a
    float3 Pos; // ���W
    float tmp;
};

cbuffer cbLight : register(b8)
{
    // �X�|�b�g���̎g�p��
    int g_SL_Count;
    float3 g_dummy;
    
    //-------------------------
    // ����
    //-------------------------
    float3 g_AmbientLight;

    //-------------------------
    // ���s��
    //-------------------------
    float3 g_DL_Dir;                // ���s���̕���
    float g_DL_DirLightShadowBias;  // ���s���̃V���h�E�}�b�v�o�C�A�X
    float3 g_DL_Color;              // ���s���̐F
    float g_DL_ShadowPower;
    row_major float4x4 g_DL_mLightVP; // ���s�� �r���[*�ˉe�ϊ��s��

    //-------------------------
    // �X�|�b�g��
    //-------------------------
    SpotLight g_SL[10];
    
    //-------------------------
    // �_��
    //-------------------------

//    PointLight g_PL[100]; // �S�Ă̕��s���f�[�^
     
    //-------------------------
    // �t�H�O
    //-------------------------
    float3  g_DistanceFogColor;
    float   g_DistanceFogDensity;
};

//----------------------------------------
// �萔�o�b�t�@[13]�F�V�X�e��
//----------------------------------------
cbuffer cbSystem : register(b13)
{
    float g_Time;
};

