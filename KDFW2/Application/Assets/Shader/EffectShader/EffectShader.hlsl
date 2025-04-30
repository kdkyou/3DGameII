//このフレームワークのシェーダー全体の決まり事
#include "inc_KdShaderCore.hlsli"

// 頂点シェーダー
float4 VS(
    //C++から送られてくる情報
    float4 pos : POSITION // この引数が何を意味するか
                           // セマンティクス
) : SV_Position // 戻り値のセマンティクス
{
    return pos;
}

// ピクセルシェーダー
// このピクセルの色を決定する
float4 PS(
    float4 svPos : SV_Position
) : SV_Target0
{
    return float4(1, 0, 1, 1); //取り合えず分かりやすい色
}