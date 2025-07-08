//このフレームワークのシェーダー全体の決まり事
#include "inc_KdShaderCore.hlsli"

//VS→PSに渡すデータ
struct VSOutPut
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD0;    //texcord0
    float4 Col : COLOR;
};


// 頂点シェーダー
VSOutPut VS(
   float4 pos : POSITION,
   float2 uv  : TEXCOORD0,
   float4 col : COLOR
)
{
    // 5/19追加
    VSOutPut ret;
    float4 retPos = pos;
    retPos = mul(retPos, g_mW); //キャラクターの位置に移動
    retPos = mul(retPos, g_mV); //カメラの逆方向に動く
    retPos = mul(retPos, g_mP); //射影（-1～+1）に変換、奥行だけ0-1
    
    ret.Pos = retPos;   //戻り値用の構造体に格納
    
    //UVが正しく動いているかどうか確認
    ret.UV = uv;       //頂点のUVを登録して補完してもらう
    
    ret.Col = col;     //色も同じく
    
    return ret;     //構造体を返す
    
}

// ピクセルシェーダー
// このピクセルの色を決定する
float4 PS(
    VSOutPut In
) : SV_Target0
{
    // 5/19追加
    float4 texcol = g_colorTex.Sample(
    g_ss_aniso_clamp, In.UV);
    
    //補完された頂点色を合成
    texcol = texcol * In.Col;     
    
    //色を使って値が設定されているか見る
    // float4 texcol = float4(In.UV, 0, 1);
    
    return texcol; //テクスチャの色反映
}