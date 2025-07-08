#pragma once

#include "KdDirectX.h"

//=================================================
// 
// ブレンドステート
// 
//=================================================
class KdBlendState
{
public:

	// ブレンドなしモードで作成
	void Create_NoBlend();

	// 半透明ブレンドモードで作成
	void Create_Alpha();

	// 加算ブレンドモードで作成
	void Create_Add();

	// 乗算ブレンドモードで作成
	void Create_Multiply();

	// デバイスにセット
	void SetToDevice(const Math::Color& blendFactor = { 0,0,0,0 }, UINT SampleMask = 0xFFFFFFFF);

	void Release()
	{
		m_bs.Reset();
	}

private:

	ComPtr<ID3D11BlendState> m_bs;

	static std::stack<std::shared_ptr<KdBlendState>> s_blendStateStack;
};

//=================================================
// 
// サンプラーステート
// 
//=================================================
class KdSamplerState
{
public:

	enum class FilterModes
	{
		Point,			// ポイントフィルタ
		Linear,			// 線形フィルタ
		Anisotropic,	// 異方性フィルタ
	};

	enum class AddressingModes
	{
		Wrap,			// ラップモード
		Clamp,			// クランプモード
	};


	void Create(FilterModes filterType, UINT maxAnisotropy, AddressingModes addressingMode, bool comparisonModel);

	// デバイスにセット
	void SetToDevice(UINT slotNo);

	void Release()
	{
		m_ss.Reset();
	}

private:

	ComPtr<ID3D11SamplerState> m_ss;

};

//=================================================
// 
// 深度ステンシルステート
// 
//=================================================
class KdDepthStencilState
{
public:

	void Create(bool zEnable, bool zWriteEnable);

	// デバイスにセット
	void SetToDevice(UINT stencilRef = 0);

	void Release()
	{
		m_ds.Reset();
	}

private:

	ComPtr<ID3D11DepthStencilState> m_ds;

};

//=================================================
// 
// ラスタライザステート
// 
//=================================================
class KdRasterizerState
{
public:

	void Create(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool depthClipEnable, bool scissorEnable);

	// デバイスにセット
	void SetToDevice();

	void Release()
	{
		m_rs.Reset();
	}

private:

	ComPtr<ID3D11RasterizerState> m_rs;

};