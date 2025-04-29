#include "KdGraphicsStates.h"

void KdBlendState::Create_NoBlend()
{
	Release();

	D3D11_BLEND_DESC desc = {};
	// 有効
	desc.RenderTarget[0].BlendEnable = TRUE;
	// 書き込みマスク　R,G,B,A全て出力する場合はALL
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	// ブレンドなし
	{
		desc.RenderTarget[0].BlendEnable = FALSE;

		// 色の合成方法
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		// アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	}

	// ステートオブジェクト作成
	D3D.GetDev()->CreateBlendState(&desc, &m_bs);
}

void KdBlendState::Create_Alpha()
{
	Release();

	D3D11_BLEND_DESC desc = {};
	// 有効
	desc.RenderTarget[0].BlendEnable = TRUE;
	// 書き込みマスク　R,G,B,A全て出力する場合はALL
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	// 半透明ブレンド
	{
		// 色の合成方法
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		// アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	}

	// ステートオブジェクト作成
	D3D.GetDev()->CreateBlendState(&desc, &m_bs);
}

void KdBlendState::Create_Add()
{
	Release();

	D3D11_BLEND_DESC desc = {};
	// 有効
	desc.RenderTarget[0].BlendEnable = TRUE;
	// 書き込みマスク　R,G,B,A全て出力する場合はALL
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	// 加算ブレンド
	{
		// 色の合成方法
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		// アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	}

	// ステートオブジェクト作成
	if (FAILED(D3D.GetDev()->CreateBlendState(&desc, &m_bs)))return;
}

void KdBlendState::Create_Multiply()
{
	Release();

	D3D11_BLEND_DESC desc = {};
	// 有効
	desc.RenderTarget[0].BlendEnable = TRUE;
	// 書き込みマスク　R,G,B,A全て出力する場合はALL
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	// 加算ブレンド
	{
		// 色の合成方法
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		// アルファの合成方法
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	}

	// ステートオブジェクト作成
	if (FAILED(D3D.GetDev()->CreateBlendState(&desc, &m_bs)))return;
}

void KdBlendState::SetToDevice(const Math::Color& blendFactor, UINT SampleMask)
{
	D3D.GetDevContext()->OMSetBlendState(m_bs.Get(), blendFactor, SampleMask);
}



void KdSamplerState::Create(FilterModes filterType, UINT maxAnisotropy, AddressingModes addressingMode, bool comparisonModel)
{
	Release();

	D3D11_SAMPLER_DESC desc = {};
	// フィルタ
	switch (filterType) {
		// Point
	case FilterModes::Point:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
		// Linear
	case FilterModes::Linear:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
		// Anisotropic
	case FilterModes::Anisotropic:
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.MaxAnisotropy = maxAnisotropy;
		break;
	}

	// アドレッシングモード
	switch (addressingMode) {
		// Wrap
	case AddressingModes::Wrap:
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
		// Clamp
	case AddressingModes::Clamp:
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	}

	// 比較モードサンプラ
	if (comparisonModel) {
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;	// とりあえず今回は線形フィルタ固定で
	}
	// 通常サンプラ
	else {
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	}

	// 
	desc.MipLODBias = 0;
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	// ステートオブジェクト作成
	ComPtr<ID3D11SamplerState> state = nullptr;
	D3D.GetDev()->CreateSamplerState(&desc, &m_ss);
}

void KdSamplerState::SetToDevice(UINT slotNo)
{
	D3D.GetDevContext()->VSSetSamplers(slotNo, 1, m_ss.GetAddressOf());
	D3D.GetDevContext()->PSSetSamplers(slotNo, 1, m_ss.GetAddressOf());
	D3D.GetDevContext()->GSSetSamplers(slotNo, 1, m_ss.GetAddressOf());
	D3D.GetDevContext()->CSSetSamplers(slotNo, 1, m_ss.GetAddressOf());
}




void KdDepthStencilState::Create(bool zEnable, bool zWriteEnable)
{
	Release();

	// Zバッファの動作設定
	D3D11_DEPTH_STENCIL_DESC desc = {};

	desc.DepthEnable = zEnable;							// 深度テストを使用する
	desc.DepthWriteMask = zWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	desc.StencilEnable = FALSE;
	desc.StencilReadMask = 0;
	desc.StencilWriteMask = 0;

	// ステートオブジェクトを作成
	D3D.GetDev()->CreateDepthStencilState(&desc, &m_ds);
}


void KdDepthStencilState::SetToDevice(UINT stencilRef)
{
	D3D.GetDevContext()->OMSetDepthStencilState(m_ds.Get(), stencilRef);
}




void KdRasterizerState::Create(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool depthClipEnable, bool scissorEnable)
{
	Release();

	// ラスタライズの動作設定
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = fillMode;
	desc.CullMode = cullMode;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0;
	desc.SlopeScaledDepthBias = 0;
	desc.DepthClipEnable = depthClipEnable;
	desc.ScissorEnable = scissorEnable;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	// ステートオブジェクト作成
	ComPtr < ID3D11RasterizerState> state = nullptr;
	D3D.GetDev()->CreateRasterizerState(&desc, &m_rs);;
}

// デバイスにセット

void KdRasterizerState::SetToDevice()
{
	D3D.GetDevContext()->RSSetState(m_rs.Get());
}

