struct VSOutput
{
    float4 Pos : SV_Position; // �ˉe���W
    float2 UV : TEXCOORD0;      // UV���W
};

//================================
// ���_�V�F�[�_
//================================
VSOutput main(float4 pos : POSITION, // ���_���W
              float2 uv : TEXCOORD0  // �e�N�X�`��UV���W
)
{
    VSOutput Out;

    Out.Pos = pos;
    Out.UV = uv;
    
    // �o��
    return Out;
}
