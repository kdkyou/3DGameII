#pragma once

#include "KdDirectX.h"

// DirectX Tex
#pragma comment(lib, "DirectXTex.lib")
#include <DirectXTex.h>

//====================================================
//
// テクスチャクラス
//
//====================================================
class KdTexture : public KdObject
{
public:

	//====================================================
	//
	// 取得
	//
	//====================================================

	// 有効なテクスチャか？
	bool								IsValid() const { return m_srv != nullptr; }

	// 画像のアスペクト比取得
	float								GetAspectRatio() const { return (float)m_desc.Width / m_desc.Height; }
	// 画像の幅を取得
	UINT								GetWidth() const { return m_desc.Width; }
	// 画像の高さを取得
	UINT								GetHeight() const { return m_desc.Height; }

	// 画像の全情報を取得
	const D3D11_TEXTURE2D_DESC&			GetInfo() const { return m_desc; }
	// ファイルパス取得(Load時のみ)
	const std::string&					GetFilepath() const { return m_filepath; }
	// RECT取得
	Math::Rectangle						GetRect() const { return {0, 0, (long)m_desc.Width, (long)m_desc.Height}; }

	// 画像リソースを取得
	ComPtr<ID3D11Texture2D>				GetResource() const { return m_resource; }

	// シェーダリソースビュー取得
	ComPtr<ID3D11ShaderResourceView>	GetSRView() const { return m_srv; }

	// シェーダリソースビュー配列取得(※テクスチャ配列専用)
	const std::vector<ComPtr<ID3D11ShaderResourceView>>&
										GetSRViewArray() const { return m_srvArray; }

	// レンダーターゲットビュー取得
	ComPtr<ID3D11RenderTargetView>		GetRTView() const { return m_rtv; }

	// 深度ステンシルビュー取得
	ComPtr<ID3D11DepthStencilView>		GetDSView() const { return m_dsv; }

	//====================================================
	//
	// 画像ファイルからテクスチャ作成
	//
	//====================================================

	// 画像ファイルを読み込む
	// ・filename		… 画像ファイル名
	// ・renderTarget	… レンダーターゲットビューを生成する(レンダーターゲットにする)
	// ・depthStencil	… 深度ステンシルビューを生成する(Zバッファにする)
	// ・generateMipmap	… ミップマップ生成する？
	bool Load(const std::string& filename, bool renderTarget = false, bool depthStencil = false, bool generateMipmap = true);

	//====================================================
	//
	// テクスチャ作成
	//
	//====================================================

	// リソースから作成
	// ・pTexture2D	… 画像リソース
	// 戻り値：true … 成功
	bool Create(ID3D11Texture2D* pTexture2D);

	// desc情報からテクスチャリソースを作成する
	// ・desc		… 作成するテクスチャリソースの情報
	// ・fillData	… バッファに書き込むデータ　nullptrだと書き込みなし
	// 戻り値：true … 成功
	bool Create(const D3D11_TEXTURE2D_DESC& desc, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	// 通常テクスチャとして作成
	// ※テクスチャリソースを作成し、ShaderResourceViewのみを作成します
	// ・w			… 画像の幅(ピクセル)
	// ・h			… 画像の高さ(ピクセル)
	// ・format		… 画像の形式　DXGI_FORMATを使用
	// ・arrayCnt	… 「テクスチャ配列」を使用する場合、その数。1で通常の1枚テクスチャ
	// ・fillData	… バッファに書き込むデータ　nullptrだと書き込みなし
	// 戻り値：true … 成功
	bool Create(uint32_t w, uint32_t h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, uint32_t arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	// レンダーテクスチャとして作成
	// ※テクスチャリソースを作成し、ShaderResourceViewのみを作成します
	// ・w			… 画像の幅(ピクセル)
	// ・h			… 画像の高さ(ピクセル)
	// ・format		… 画像の形式　DXGI_FORMATを使用
	// ・arrayCnt	… 「テクスチャ配列」を使用する場合、その数。1で通常の1枚テクスチャ
	// ・fillData	… バッファに書き込むデータ　nullptrだと書き込みなし
	// 戻り値：true … 成功
	bool CreateRenderTarget(uint32_t w, uint32_t h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, uint32_t arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	// 深度ステンシルテクスチャ(Zバッファ)として作成
	// ・w			… 画像の幅(ピクセル)
	// ・h			… 画像の高さ(ピクセル)
	// ・format		… 画像の形式　DXGI_FORMATを使用
	// ・arrayCnt	… 「テクスチャ配列」を使用する場合、その数。1で通常の1枚テクスチャ
	// ・fillData	… バッファに書き込むデータ　nullptrだと書き込みなし
	// 戻り値：true … 成功
	bool CreateDepthStencil(uint32_t w, uint32_t h, DXGI_FORMAT format = DXGI_FORMAT_R24G8_TYPELESS, uint32_t arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr);

	// ビューから作成
	void SetSRView(ID3D11ShaderResourceView* srv);

	// RenderTextureの内容をクリア
	void ClearRT(const Math::Color& color = {0,0,0,1})
	{
		if (m_rtv == nullptr)return;

		D3D.GetDevContext()->ClearRenderTargetView(m_rtv.Get(), color);
	}
	// DepthStencilの内容をクリア
	void ClearDepth(float depth = 1.0f, uint8_t stencil = 0)
	{
		if (m_dsv == nullptr)return;

		D3D.GetDevContext()->ClearDepthStencilView(m_dsv .Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
	}

	// 画像をコピーする(条件あり CopyResource関数を参照)
	// ・異なるリソースである必要があります。
	// ・同じ型である必要があります。
	// ・同じ寸法(必要に応じて幅、高さ、深さ、サイズを含む) を持つ必要があります。
	// ・互換性のある DXGI 形式が必要です。
	void CopyTo(const KdTexture& dest)
	{
		D3D.GetDevContext()->CopyResource(dest.GetResource().Get(), m_resource.Get());
	}

	// 
	KdTexture() {}

	KdTexture(const std::string& filepath)
	{
		Load(filepath);
	}

	//====================================================
	// 解放
	//====================================================
	void Release();

	// 
	~KdTexture()
	{
		Release();
	}

protected:

	// 画像用
	ComPtr<ID3D11Texture2D>				m_resource = nullptr;
	// 深度バッファ用
	ComPtr<ID3D11Texture2D>				m_depth = nullptr;

	// 画像情報
	D3D11_TEXTURE2D_DESC				m_desc = {};

	// シェーダリソースビュー(読み取り用)
	ComPtr<ID3D11ShaderResourceView>	m_srv = nullptr;
	// レンダーターゲットビュー(書き込み用)
	ComPtr<ID3D11RenderTargetView>		m_rtv = nullptr;
	// 深度ステンシルビュー(Zバッファ用)
	ComPtr<ID3D11DepthStencilView>		m_dsv = nullptr;
	// アンオーダードアクセスビュー
	//※未実装※

	// テクスチャ配列時専用
	std::vector<ComPtr<ID3D11ShaderResourceView>>	m_srvArray;


	// 画像ファイル名(Load時専用)
	std::string							m_filepath;

	static ID3D11RenderTargetView*		s_tempRTVs[8];

private:
	// コピー禁止用
	KdTexture(const KdTexture& src) = delete;
	void operator=(const KdTexture& src) = delete;
};


//====================================================
// 
// レンダーテクスチャクラス
// 
// ・KdTextureにDepth用のテクスチャを付けたしたもの
// 
//====================================================
class KdRenderTexture : public KdTexture
{
public:

	const std::shared_ptr<KdTexture> GetDepth() const { return m_depth; }


	// レンダーターゲットとZバッファを作成する
	// ・w			… 画像の幅(ピクセル)
	// ・h			… 画像の高さ(ピクセル)
	// ・format		… 画像の形式　DXGI_FORMATを使用
	// ・arrayCnt	… 「テクスチャ配列」を使用する場合、その数。1で通常の1枚テクスチャ
	// ・fillData	… バッファに書き込むデータ　nullptrだと書き込みなし
	// ・depthFormat… Zバッファのフォーマット(DXGI_FORMAT_UNKNOWNで作成しない)
	// 戻り値：true … 成功
	bool CreateRenderTexture(uint32_t w, uint32_t h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, uint32_t arrayCnt = 1, const D3D11_SUBRESOURCE_DATA* fillData = nullptr, DXGI_FORMAT depthFormat = DXGI_FORMAT_R24G8_TYPELESS)
	{
		Release();

		// レンダーターゲット
		if (format != DXGI_FORMAT_UNKNOWN)
		{
			if (CreateRenderTarget(w, h, format, arrayCnt, fillData) == false)
			{
				Release();
				return false;
			}
		}

		// 深度バッファ
		if (depthFormat != DXGI_FORMAT_UNKNOWN)
		{
			m_depth = std::make_shared<KdTexture>();
			if (m_depth->CreateDepthStencil(w, h, depthFormat, 1, nullptr) == false)
			{
				return false;
			}
		}
		return true;
	}

	// 内容をクリア
	void Clear(const Math::Color& color = { 0,0,0,1 }, float depth = 1.0f, uint8_t stencil = 0)
	{
		ClearRT(color);

		if (m_depth != nullptr)
		{
			m_depth->ClearDepth(depth, stencil);
		}
	}

	void CopyTo(const KdRenderTexture& dest)
	{
		// カラーバッファ コピー
		D3D.GetDevContext()->CopyResource(dest.GetResource().Get(), GetResource().Get());
		// Zバッファ コピー
		if (dest.GetDepth() != nullptr && dest.GetDepth()->IsValid())
		{
			D3D.GetDevContext()->CopyResource(dest.GetDepth()->GetResource().Get(), m_depth->GetResource().Get());
		}
	}

	KdRenderTexture() {}

	void Release()
	{
		KdTexture::Release();

		m_depth = nullptr;
	}


private:

	// 深度バッファ
	std::shared_ptr<KdTexture> m_depth;

private:
	// コピー禁止用
	KdRenderTexture(const KdRenderTexture& src) = delete;
	void operator=(const KdRenderTexture& src) = delete;
};

// viewから画像情報を取得する
inline void KdGetTextureInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc)
{
	outDesc = {};

	ID3D11Resource* res;
	view->GetResource(&res);

	ID3D11Texture2D* tex2D;
	if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&tex2D)))
	{
		tex2D->GetDesc(&outDesc);
		tex2D->Release();
	}
	res->Release();
}
