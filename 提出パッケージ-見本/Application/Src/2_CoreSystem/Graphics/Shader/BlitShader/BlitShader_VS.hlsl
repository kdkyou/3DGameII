struct VSOutput
{
    float4 Pos : SV_Position; // 射影座標
    float2 UV : TEXCOORD0;      // UV座標
};

//================================
// 頂点シェーダ
//================================
VSOutput main(float4 pos : POSITION, // 頂点座標
              float2 uv : TEXCOORD0  // テクスチャUV座標
)
{
    VSOutput Out;

    Out.Pos = pos;
    Out.UV = uv;
    
    // 出力
    return Out;
}
