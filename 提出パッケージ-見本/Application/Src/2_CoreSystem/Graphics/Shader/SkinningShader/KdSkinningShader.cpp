#include "KdSkinningShader.h"

bool KdSkinningShader::Initialize()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "Skinning_VS.csh"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		1,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		2,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SKININDEX",  0, DXGI_FORMAT_R16G16B16A16_UINT,	3,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	4,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&inputLayout[0],			// 入力エレメント先頭アドレス
			(UINT)inputLayout.size(),		// 入力エレメント数
			&compiledBuffer[0],				// 頂点バッファのバイナリデータ
			sizeof(compiledBuffer),			// 上記のバッファサイズ
			&m_inputLayout))					// 
		) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}

		// ストリームアウトプット＆ジオメトリシェーダー
		std::vector<D3D11_SO_DECLARATION_ENTRY> soEntrys =
		{
		  { 0, "SV_Position",	0, 0, 3, kStreamOutputSlotTypes::Pos },
		  { 0, "TANGENT",		0, 0, 3, kStreamOutputSlotTypes::Tangent },
		  { 0, "NORMAL",		0, 0, 3, kStreamOutputSlotTypes::Normal },
		};

		std::vector<uint32_t> strides =
		{
			12,
			12,
			12,
		};

		if (FAILED(D3D.GetDev()->CreateGeometryShaderWithStreamOutput(compiledBuffer, sizeof(compiledBuffer),
			&soEntrys[0], (uint32_t)soEntrys.size(),
			&strides[0], (uint32_t)strides.size(),
			D3D11_SO_NO_RASTERIZED_STREAM,
			nullptr,
			&m_GS)))
		{
			assert(0 && "CreateGeometryShaderWithStreamOutput 失敗");
			Release();
			return false;
		}
	}

	return true;
}

void KdSkinningShader::Release()
{
	m_VS.Reset();
	m_GS.Reset();
	m_inputLayout.Reset();
}


void KdSkinningShader::DrawToStreamOutput(uint32_t vertexCount, const std::vector<ID3D11Buffer*>& soVBs)
{
	// プリミティブトポロジーをセット(POINTLIST…頂点単位)
	D3D	.GetDevContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// 頂点シェーダをセット
	D3D.GetDevContext()->VSSetShader(m_VS.Get(), 0, 0);
	// ジオメトリシェーダをセット
	D3D.GetDevContext()->GSSetShader(m_GS.Get(), 0, 0);
	// ピクセルシェーダーは使用しない
	D3D.GetDevContext()->PSSetShader(nullptr, 0, 0);

	// 頂点レイアウトをセット
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout.Get());

	// Stream Output出力バッファをセット
	uint32_t soOffset[D3D11_SO_BUFFER_SLOT_COUNT] = {0, 0, 0, 0};
	D3D.GetDevContext()->SOSetTargets((uint32_t)soVBs.size(), &soVBs[0], &soOffset[0]);

	// 描画
	D3D.GetDevContext()->Draw(vertexCount, 0);

	// ジオメトリシェーダーを解除
	D3D.GetDevContext()->GSSetShader(nullptr, 0, 0);

	// SO出力バッファを解除
	static ID3D11Buffer* nullBuffer[D3D11_SO_BUFFER_SLOT_COUNT] = { nullptr, nullptr, nullptr, nullptr };
	D3D.GetDevContext()->SOSetTargets((uint32_t)soVBs.size(), nullBuffer, &soOffset[0]);
}

