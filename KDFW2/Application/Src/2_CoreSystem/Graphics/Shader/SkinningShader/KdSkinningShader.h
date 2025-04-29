#pragma once

#include "../../KdDirectX.h"

class KdSkinningShader
{
public:

	// 初期化
	bool Initialize();

	// 解放
	void Release();

	// Stream Output機能で、頂点を描画する。
	// ・vertexCount	：描画する頂点数
	// ・soVBs			：Stream Output出力先の、頂点バッファたち
	void DrawToStreamOutput(uint32_t vertexCount, const std::vector<ID3D11Buffer*>& soVBs);

	// ボーン配列 定数バッファ
	struct CBufferData
	{
		KdMatrix		mBoneMatrix[1024];
	};

	// 
	enum kStreamOutputSlotTypes
	{
		Pos = 0,
		Tangent = 1,
		Normal = 2,
	};

private:


	// 3Dモデル用シェーダ
	ComPtr<ID3D11VertexShader>		m_VS = nullptr;				// 頂点シェーダー
	ComPtr<ID3D11GeometryShader>	m_GS = nullptr;				// ジオメトリシェーダー
	ComPtr<ID3D11InputLayout>		m_inputLayout = nullptr;		// 頂点入力レイアウト


};