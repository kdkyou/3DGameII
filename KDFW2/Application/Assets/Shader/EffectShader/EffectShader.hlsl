//���̃t���[�����[�N�̃V�F�[�_�[�S�̂̌��܂莖
#include "inc_KdShaderCore.hlsli"

// ���_�V�F�[�_�[
float4 VS(
    //C++���瑗���Ă�����
    float4 pos : POSITION // ���̈����������Ӗ����邩
                           // �Z�}���e�B�N�X
) : SV_Position // �߂�l�̃Z�}���e�B�N�X
{
    return pos;
}

// �s�N�Z���V�F�[�_�[
// ���̃s�N�Z���̐F�����肷��
float4 PS(
    float4 svPos : SV_Position
) : SV_Target0
{
    return float4(1, 0, 1, 1); //��荇����������₷���F
}