// �e�N�X�`��
Texture2D g_tex : register(t0); // �x�[�X�J���[�e�N�X�`��
// �T���v��
SamplerState g_ss : register(s0);

struct PSInput
{
    float4 Pos : SV_Position; // �ˉe���W
    float2 UV : TEXCOORD0; // UV���W
};
//================================
//
// �s�N�Z���V�F�[�_
//
//================================
float4 main(PSInput In) : SV_Target0
{
    return g_tex.Sample(g_ss, In.UV);
}
