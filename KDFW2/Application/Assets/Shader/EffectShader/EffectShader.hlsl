//���̃t���[�����[�N�̃V�F�[�_�[�S�̂̌��܂莖
#include "inc_KdShaderCore.hlsli"

//VS��PS�ɓn���f�[�^
struct VSOutPut
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD0;    //texcord0
    float4 Col : COLOR;
};


// ���_�V�F�[�_�[
VSOutPut VS(
   float4 pos : POSITION,
   float2 uv  : TEXCOORD0,
   float4 col : COLOR
)
{
    // 5/19�ǉ�
    VSOutPut ret;
    float4 retPos = pos;
    retPos = mul(retPos, g_mW); //�L�����N�^�[�̈ʒu�Ɉړ�
    retPos = mul(retPos, g_mV); //�J�����̋t�����ɓ���
    retPos = mul(retPos, g_mP); //�ˉe�i-1�`+1�j�ɕϊ��A���s����0-1
    
    ret.Pos = retPos;   //�߂�l�p�̍\���̂Ɋi�[
    
    //UV�������������Ă��邩�ǂ����m�F
    ret.UV = uv;       //���_��UV��o�^���ĕ⊮���Ă��炤
    
    ret.Col = col;     //�F��������
    
    return ret;     //�\���̂�Ԃ�
    
}

// �s�N�Z���V�F�[�_�[
// ���̃s�N�Z���̐F�����肷��
float4 PS(
    VSOutPut In
) : SV_Target0
{
    // 5/19�ǉ�
    float4 texcol = g_colorTex.Sample(
    g_ss_aniso_clamp, In.UV);
    
    //�⊮���ꂽ���_�F������
    texcol = texcol * In.Col;     
    
    //�F���g���Ēl���ݒ肳��Ă��邩����
    // float4 texcol = float4(In.UV, 0, 1);
    
    return texcol; //�e�N�X�`���̐F���f
}