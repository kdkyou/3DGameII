//----------------------------------------
// 定数バッファ[1]：描画単位
//----------------------------------------
cbuffer cbBones : register(b0)
{
    row_major float4x4 g_mBones[1024];
};

// 頂点バッファ出力用
struct VSOutput
{
    float4 Pos : SV_Position;
    float3 wT : TANGENT;
    float3 wN : NORMAL;
};

//================================
// 頂点シェーダ
//================================
VSOutput main(  float4 pos : POSITION,      // 頂点座標
                float3 tangent : TANGENT,   // 接線ベクトル
                float3 normal : NORMAL,     // 法線
                uint4 skinIndex : SKININDEX,
                float4 skinWeight : SKINWEIGHT
)
{
    //------------------------------
    // スキニング処理
    // この頂点に影響している複数のボーンの行列を使用して
    // posやnormalなどを変換する
    //------------------------------
    row_major float4x4 mBones = 0; // 最終的なワールド行列
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        mBones += g_mBones[skinIndex[i]] * skinWeight[i];
    }
    // ボーン行列で変換(まだローカル座標系)
    pos = mul(pos, mBones);
    tangent = mul(tangent, (float3x3) mBones);
    normal = mul(normal, (float3x3) mBones);
    //------------------------------

    // 出力データ作成
    VSOutput Out = (VSOutput) 0;

    Out.Pos = pos;
    Out.wT = tangent;
    Out.wN = normal;

    // 出力
    return Out;
}
