#include "KdDirect3D.h"

#include "KdBuffer.h"
#include "KdTexture.h"

void KdDirect3D::GetViewport(Math::Viewport & out) const
{
	UINT numVPs = 1;
	D3D11_VIEWPORT vp;
	m_pDeviceContext->RSGetViewports(&numVPs, &vp);
	out = vp;

}

uint32_t KdDirect3D::GetWidth() const { return m_backBuffer->GetWidth(); }

uint32_t KdDirect3D::GetHeight() const { return m_backBuffer->GetHeight(); }

bool KdDirect3D::Initialize(HWND hWnd, uint32_t w, uint32_t h, bool deviceDebug, std::string& errMsg)
{
	HRESULT hr;

	//=====================================================
	// ファクトリー作成(ビデオ グラフィックの設定の列挙や指定に使用されるオブジェクト)
	//=====================================================
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(m_pGIFactory.GetAddressOf()));
	if (FAILED(hr)) {
		errMsg = "ファクトリー作成失敗";

		Release();
		return false;
	}

	

	//=====================================================
	//デバイス生成(主にリソース作成時に使用するオブジェクト)
	//=====================================================
	UINT creationFlags = 0;

	if (deviceDebug) {
		// Direct3Dのデバッグを有効にする(すごく重いが細かいエラーがわかる)
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	D3D_FEATURE_LEVEL featureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_1,	// Direct3D 11.1  ShaderModel 5
		D3D_FEATURE_LEVEL_11_0,	// Direct3D 11    ShaderModel 5
		D3D_FEATURE_LEVEL_10_1,	// Direct3D 10.1  ShaderModel 4
		D3D_FEATURE_LEVEL_10_0,	// Direct3D 10.0  ShaderModel 4
		D3D_FEATURE_LEVEL_9_3,	// Direct3D 9.3   ShaderModel 3
		D3D_FEATURE_LEVEL_9_2,	// Direct3D 9.2   ShaderModel 3
		D3D_FEATURE_LEVEL_9_1,	// Direct3D 9.1   ShaderModel 3
	};

	// デバイスとでデバイスコンテキストを作成
	D3D_FEATURE_LEVEL futureLevel;
	hr = D3D11CreateDevice(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				creationFlags,
				featureLevels,
				_countof(featureLevels),
				D3D11_SDK_VERSION,
				&m_pDevice,
				&futureLevel,
				&m_pDeviceContext);
	if FAILED( hr )
	{
		errMsg = "Direct3D11デバイス作成失敗";

		Release();
		return false;
	}

	//=====================================================
	// スワップチェイン作成(フロントバッファに表示可能なバックバッファを持つもの)
	//=====================================================
	DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc = {};		// スワップチェーンの設定データ
	DXGISwapChainDesc.BufferDesc.Width = w;
	DXGISwapChainDesc.BufferDesc.Height = h;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Numerator  = 0;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	DXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	DXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;// DXGI_MODE_SCALING_CENTERED;	// DXGI_MODE_SCALING_UNSPECIFIED
	DXGISwapChainDesc.SampleDesc.Count = 1;
	DXGISwapChainDesc.SampleDesc.Quality = 0;
	DXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	DXGISwapChainDesc.BufferCount = 2;
	DXGISwapChainDesc.OutputWindow = hWnd;
	DXGISwapChainDesc.Windowed = TRUE;
	DXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	DXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if(FAILED(m_pGIFactory->CreateSwapChain(m_pDevice.Get(), &DXGISwapChainDesc, &m_pGISwapChain))){
		errMsg = "スワップチェイン作成失敗";

		Release();
		return false;
	}

	// スワップチェインからバックバッファ取得
	ID3D11Texture2D* pBackBuffer;
	if (FAILED(m_pGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer))) {
		errMsg = "バックバッファ取得失敗";

		Release();
		return false;
	}

	//=========================================================
	// 作成されたバックバッファはそのまま使用しZバッファは作成する
	//=========================================================
	// バックバッファリソースからビューを作成
	m_backBuffer = std::make_shared<KdRenderTexture>();
	if (m_backBuffer->Create(pBackBuffer) == false)
	{
		errMsg = "バックバッファ作成失敗";
		Release();
		return false;
	}

	/*
	// ALT+Enterでフルスクリーン不可にする
	{
		IDXGIDevice* pDXGIDevice;
		m_pDevice->QueryInterface<IDXGIDevice>(&pDXGIDevice);

		IDXGIAdapter* pDXGIAdapter;
		pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

		IDXGIFactory* pIDXGIFactory;
		pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

		pIDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		pDXGIDevice->Release();
		pDXGIAdapter->Release();
		pIDXGIFactory->Release();
	}
	*/

	//=========================================================
	// ビューポートの設定
	//=========================================================
	{
		// ビューポートの設定
		D3D11_VIEWPORT vp;
		vp.Width = (float)w;
		vp.Height = (float)h;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pDeviceContext->RSSetViewports(1, &vp);
	}

	//=========================================================
	// 1x1の白テクスチャ作成
	//=========================================================
	{
		// 0xAABBGGRR
		auto col = Math::Color(1, 1, 1, 1).RGBA();
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = &col;
		srd.SysMemPitch = 4;
		srd.SysMemSlicePitch = 0;

		m_texDotWhite = std::make_shared<KdTexture>();
		m_texDotWhite->Create(1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, 1, &srd);
	}

	//=========================================================
	// 1x1のZ方向を向けた法線マップテクスチャ作成
	//=========================================================
	{
		// 0xAABBGGRR
		auto col = Math::Color(0.5f, 0.5f, 1, 1).RGBA();
		D3D11_SUBRESOURCE_DATA srd;
		srd.pSysMem = &col;
		srd.SysMemPitch = 4;
		srd.SysMemSlicePitch = 0;

		m_texDotNormal = std::make_shared<KdTexture>();
		m_texDotNormal->Create(1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, 1, &srd);
	}

	// DrawVertices用頂点バッファを作成
	UINT bufferSize = 80;
	for (int i = 0; i < 10; i++)	// 
	{
		m_tempFixedVertexBuffer[i] = std::make_shared<KdBuffer>();
		m_tempFixedVertexBuffer[i]->Create(D3D11_BIND_VERTEX_BUFFER, bufferSize, D3D11_USAGE_DYNAMIC, 0, nullptr);

		bufferSize *= 2;	// 容量を倍にしていく
	}

	m_tempVertexBuffer = std::make_shared<KdBuffer>();

	return true;
}

void KdDirect3D::Release()
{
	for (auto&& n : m_tempFixedVertexBuffer)
	{
		n = nullptr;
	}

	m_tempVertexBuffer = nullptr;

	m_backBuffer = nullptr;
	m_texDotWhite = nullptr;
	m_texDotNormal = nullptr;

	for (int i = 0; i < 8; i++)
	{
		m_setRTVs[i].Reset();
	}
	m_setDSV.Reset();

	m_pGISwapChain.Reset();
//	m_pGIAdapter.Reset();
	m_pGIFactory.Reset();
	m_pDeviceContext.Reset();;
	m_pDevice.Reset();

}

void KdDirect3D::SetRenderTargets(std::vector<KdTexture*> rts, const KdTexture* depth)
{
	// 
	for (int i = 0; i < 8; i++)
	{
		if (i < rts.size())
		{
			m_setRTVs[i] = rts[i] != nullptr ? rts[i]->GetRTView() : nullptr;
		}
		else
		{
			m_setRTVs[i].Reset();
		}
	}

	// 
	if (depth != nullptr)
	{
		m_setDSV = depth->GetDSView();
	}
	else
	{
		m_setDSV.Reset();
	}

	SetRenderTargetsAndDepth();
}

void KdDirect3D::SetRenderTargets(std::vector<KdTexture*> rts)
{
	for (int i = 0; i < 8; i++)
	{
		if (i < rts.size())
		{
			m_setRTVs[i] = rts[i] != nullptr ? rts[i]->GetRTView() : nullptr;
		}
		else
		{
			m_setRTVs[i].Reset();
		}
	}

	SetRenderTargetsAndDepth();
}

void KdDirect3D::SetRenderTarget(uint32_t index, KdTexture* rt)
{
	m_setRTVs[index] = rt != nullptr ? rt->GetRTView() : nullptr;

	SetRenderTargetsAndDepth();
}

void KdDirect3D::SetDepthStencil(const KdTexture* depth)
{
	if (depth != nullptr)
	{
		m_setDSV = depth->GetDSView();
	}
	else
	{
		m_setDSV.Reset();
	}

	SetRenderTargetsAndDepth();
}

void KdDirect3D::SetRenderTargetsAndDepth()
{
	ID3D11RenderTargetView* rtvs[8] = {};
	for (int i = 0; i < 8; i++)
	{
		rtvs[i] = m_setRTVs[i].Get();
	}
	m_pDeviceContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtvs, m_setDSV.Get());

	// ビューポートも設定(0番目の情報を使用
	if (rtvs[0] != nullptr)
	{
		D3D11_TEXTURE2D_DESC desc = {};
		KdGetTextureInfo(rtvs[0], desc);

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = (float)desc.Width;
		vp.Height = (float)desc.Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		GetDevContext()->RSSetViewports(1, &vp);
	}

}


void KdDirect3D::SetViewportFromRTV(ID3D11RenderTargetView* rtv)
{
	if (rtv == nullptr)return;

	// 画像情報取得
	D3D11_TEXTURE2D_DESC desc;
	KdGetTextureInfo(rtv, desc);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (float)desc.Width;
	vp.Height = (float)desc.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	GetDevContext()->RSSetViewports(1, &vp);
}

void KdDirect3D::DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology,int vertexCount, const void* pVertexStream, UINT stride)
{
	
	// プリミティブトポロジーをセット
	GetDevContext()->IASetPrimitiveTopology(topology);

	// 全頂点の総バイトサイズ
	UINT totalSize = vertexCount * stride;


	// 最適な固定長バッファを検索
	std::shared_ptr<KdBuffer> buffer = nullptr;
	for (auto&& n : m_tempFixedVertexBuffer)
	{
		if (totalSize < n->GetBufferSize())
		{
			buffer = n;
			break;
		}
	}
	// 可変長のバッファを使用
	if (buffer == nullptr)
	{
		buffer = m_tempVertexBuffer;

		// 頂点バッファのサイズが小さいときは、リサイズのため再作成する
		if (m_tempVertexBuffer->GetBufferSize() < totalSize) {
			m_tempVertexBuffer->Create(D3D11_BIND_VERTEX_BUFFER, totalSize, D3D11_USAGE_DYNAMIC, 0, nullptr);
		}
	}

	//============================================================
	//
	// 単純なDISCARDでの書き込み。
	//  DISCARDは、新たなバッファを内部で作成し、前回のバッファは使い終わると無効にするものです。
	//  つまり書き込む度に新規のバッファになる感じです。
	//  バッファのサイズが大きいと、その分新規のバッファを作成する時間がかかってしまいます。
	//  これを改善したい場合は、「DISCARDとNO_OVERWRITEの組み合わせ技」の方法で行うほうがよいです。
	//
	//============================================================

	// 全頂点をバッファに書き込み(DISCARD指定)
	buffer->WriteData(pVertexStream, totalSize);

	// 頂点バッファーをデバイスへセット
	{
		UINT offset = 0;
		D3D.GetDevContext()->IASetVertexBuffers(0, 1, buffer->GetBuffer().GetAddressOf(), &stride, &offset);
	}

	// 描画
	GetDevContext()->Draw(vertexCount, 0);
	
}

void KdDirect3D::SetTextureToVS(uint32_t slotNo, KdTexture* tex) const
{
	if (tex != nullptr && tex->GetSRView() != nullptr)
	{
		m_pDeviceContext->VSSetShaderResources(slotNo, 1, tex->GetSRView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* srvs[] = { nullptr };
		m_pDeviceContext->VSSetShaderResources(slotNo, 1, srvs);
	}
}

void KdDirect3D::SetTextureToPS(uint32_t slotNo, KdTexture* tex) const
{
	if (tex != nullptr && tex->GetSRView() != nullptr)
	{
		m_pDeviceContext->PSSetShaderResources(slotNo, 1, tex->GetSRView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* srvs[] = { nullptr };
		m_pDeviceContext->PSSetShaderResources(slotNo, 1, srvs);
	}
}

void KdDirect3D::SetTextureToGS(uint32_t slotNo, KdTexture* tex) const
{
	if (tex != nullptr && tex->GetSRView() != nullptr)
	{
		m_pDeviceContext->GSSetShaderResources(slotNo, 1, tex->GetSRView().GetAddressOf());
	}
	else
	{
		ID3D11ShaderResourceView* srvs[] = { nullptr };
		m_pDeviceContext->GSSetShaderResources(slotNo, 1, srvs);
	}
}


