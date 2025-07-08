#include "KdBlitShader.h"

struct Vertex
{
	KdVector3 Pos;
	KdVector2 UV;
};


bool KdBlitShader::Initialize()
{
	//-------------------------------------
	// 頂点シェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "BlitShader_VS.csh"

		// 頂点シェーダー作成
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "頂点シェーダー作成失敗");
			Release();
			return false;
		}

		// １頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0,  12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// 頂点入力レイアウト作成
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0],			// 入力エレメント先頭アドレス
			(UINT)layout.size(),		// 入力エレメント数
			&compiledBuffer[0],				// 頂点バッファのバイナリデータ
			sizeof(compiledBuffer),			// 上記のバッファサイズ
			&m_inputLayout))					// 
		) {
			assert(0 && "CreateInputLayout失敗");
			Release();
			return false;
		}

	}

	//-------------------------------------
	// ピクセルシェーダ
	//-------------------------------------
	{
		// コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "BlitShader_PS.csh"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "ピクセルシェーダー作成失敗");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// 頂点作成
	//-------------------------------------

	Vertex vertex[] = {
		{ {-1,-1, 0}, { 0, 1} },	// 左下
		{ {-1, 1, 0}, { 0, 0} },	// 左上
		{ { 1,-1, 0}, { 1, 1} },	// 右下
		{ { 1, 1, 0}, { 1, 0} },	// 右上
	};

	m_vb = std::make_shared<KdBuffer>();
	m_vb->Create(D3D11_BIND_VERTEX_BUFFER, sizeof(Vertex) * 4, D3D11_USAGE_DYNAMIC, 0, nullptr);
	m_vb->WriteData(vertex, sizeof(vertex));

	return true;
}

void KdBlitShader::Draw(KdTexture* srcTexture, KdRenderTexture* destTexture, KdMaterial* material, uint32_t passIdx)
{
	// ステートのセット
	KdShaderManager::GetInstance().m_ds_ZDisable_ZWriteDisable->SetToDevice();

	// マテリアルがある場合は、こっちのシェーダーを使用する
	if (material != nullptr)
	{
		// 頂点シェーダが無い場合は、標準のものを使用する
		if (material->IsValidVS(passIdx) == false)
		{
			// 頂点シェーダをセット
			D3D.GetDevContext()->VSSetShader(m_VS.Get(), 0, 0);
			// 頂点レイアウトをセット
			D3D.GetDevContext()->IASetInputLayout(m_inputLayout.Get());
		}

		// ピクセルシェーダをセット
		material->SetToDevice(passIdx);
	}
	else
	{
		// 頂点シェーダをセット
		D3D.GetDevContext()->VSSetShader(m_VS.Get(), 0, 0);
		// 頂点レイアウトをセット
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout.Get());
		// ピクセルシェーダをセット
		D3D.GetDevContext()->PSSetShader(m_PS.Get(), 0, 0);

	}

	// プリミティブトポロジーをセット
	D3D.GetDevContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// レンダーターゲットを設定
	if (destTexture != nullptr)
	{
		D3D.SetRenderTargets({ }, nullptr);
	}

	// テクスチャセット
	if (srcTexture != nullptr)
	{
		D3D.SetTextureToPS(0, srcTexture);
	}

	// レンダーターゲットを設定
	if (destTexture != nullptr)
	{
		D3D.SetRenderTargets({ destTexture }, nullptr);
	}

	// 頂点バッファーをデバイスへセット
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		D3D.GetDevContext()->IASetVertexBuffers(0, 1, m_vb->GetBuffer().GetAddressOf(), &stride, &offset);
	}

	// 描画
	D3D.GetDevContext()->Draw(4, 0);
}


void KdBlitShader::Release()
{
    m_VS.Reset();
    m_inputLayout.Reset();
    m_PS.Reset();

	m_vb = nullptr;
}
