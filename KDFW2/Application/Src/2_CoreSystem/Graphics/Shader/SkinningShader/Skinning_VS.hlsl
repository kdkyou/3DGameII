//----------------------------------------
// �萔�o�b�t�@[1]�F�`��P��
//----------------------------------------
cbuffer cbBones : register(b0)
{
    row_major float4x4 g_mBones[1024];
};

// ���_�o�b�t�@�o�͗p
struct VSOutput
{
    float4 Pos : SV_Position;
    float3 wT : TANGENT;
    float3 wN : NORMAL;
};

//================================
// ���_�V�F�[�_
//================================
VSOutput main(  float4 pos : POSITION,      // ���_���W
                float3 tangent : TANGENT,   // �ڐ��x�N�g��
                float3 normal : NORMAL,     // �@��
                uint4 skinIndex : SKININDEX,
                float4 skinWeight : SKINWEIGHT
)
{
    //------------------------------
    // �X�L�j���O����
    // ���̒��_�ɉe�����Ă��镡���̃{�[���̍s����g�p����
    // pos��normal�Ȃǂ�ϊ�����
    //------------------------------
    row_major float4x4 mBones = 0; // �ŏI�I�ȃ��[���h�s��
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        mBones += g_mBones[skinIndex[i]] * skinWeight[i];
    }
    // �{�[���s��ŕϊ�(�܂����[�J�����W�n)
    pos = mul(pos, mBones);
    tangent = mul(tangent, (float3x3) mBones);
    normal = mul(normal, (float3x3) mBones);
    //------------------------------

    // �o�̓f�[�^�쐬
    VSOutput Out = (VSOutput) 0;

    Out.Pos = pos;
    Out.wT = tangent;
    Out.wN = normal;

    // �o��
    return Out;
}
