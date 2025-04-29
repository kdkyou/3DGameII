#pragma once

#include "../../KdDirectX.h"

class KdBlitShader
{
public:

	// 初期化
	bool Initialize();

	// 解放
	void Release();

	// 現在のRT全体に、矩形を描画する
	// ・texture … Slot0にセットするテクスチャ
	// ・material … 指定すると、子のマテリアルのシェーダーを使用する。nullだと標準シェーダーを使用する
	// ・passIdx … materialのパス番号を指定
	void Draw(KdTexture* srcTexture, KdRenderTexture* destTexture, KdMaterial* material = nullptr, uint32_t passIdx = 0);

private:


	// 3Dモデル用シェーダ
	ComPtr<ID3D11VertexShader>	m_VS = nullptr;				// 頂点シェーダー
	ComPtr<ID3D11InputLayout>	m_inputLayout = nullptr;		// 頂点入力レイアウト
	ComPtr<ID3D11PixelShader>	m_PS = nullptr;				// ピクセルシェーダー

	// 頂点バッファ
	std::shared_ptr<KdBuffer>	m_vb;
};