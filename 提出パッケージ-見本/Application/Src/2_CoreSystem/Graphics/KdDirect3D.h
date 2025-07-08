#pragma once

#include "KdDirectX.h"

#include "Math/KdMath.h"

class KdTexture;
class KdRenderTexture;
class KdBuffer;

//=======================================================================================
//
// Direct3Dクラス
//
//	Direct3D11に必要な変数や、よく行う操作をまとめています。
//
//
//=======================================================================================
class KdDirect3D {
public:

	//==============================================================
	//
	// 取得
	//
	//==============================================================

	// Direct3Dデバイス取得
	ComPtr<ID3D11Device>		GetDev() { return m_pDevice; }

	// Direct3Dデバイスコンテキスト取得
	ComPtr<ID3D11DeviceContext> GetDevContext() { return m_pDeviceContext; }

	// SwapChain取得
	ComPtr<IDXGISwapChain>		GetSwapChain() { return m_pGISwapChain; }

	// バックバッファ＋Zバッファ
	std::shared_ptr<KdRenderTexture>	GetBackBuffer() { return m_backBuffer; }

	// ビューポート取得
	void						GetViewport(Math::Viewport& out) const;

	// 1x1 白テクスチャ取得
	std::shared_ptr<KdTexture>	GetDotWhiteTex() const { return m_texDotWhite; }
	// 1x1 デフォルト法線マップ
	std::shared_ptr<KdTexture>	GetDotNormalTex() const { return m_texDotNormal; }

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	//==============================================================
	//
	// 初期化・解放
	//
	//==============================================================

	// 初期化
	// ・hWnd			… ウィンドウのハンドル
	// ・w				… X解像度
	// ・h				… Y解像度
	// ・debugDevice	… デバッグモード
	// ・errMsg			… (出力)エラーメッセージが入ってくる
	bool Initialize(HWND hWnd, uint32_t w, uint32_t h, bool deviceDebug, std::string& errMsg);

	// 解放
	void Release();

	//==============================================================

	//==============================================================
	//
	// 描画
	//
	//==============================================================

	// 登録されている全てのRenderTargetと、Zバッファを解除する
	void RemoveAllRenderTargets()
	{
		ID3D11RenderTargetView* rtvs[8];
		for (int i = 0; i < 8; i++)
		{
			m_setRTVs[i].Reset();
			rtvs[i] = nullptr;
		}
		m_setDSV.Reset();

		m_pDeviceContext->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtvs, nullptr);
	}


	// レンダーターゲットとZバッファを設定
	void SetRenderTargets(std::vector<KdTexture*> rts, const KdTexture* depth);

	// レンダーターゲットを設定
	void SetRenderTargets(std::vector<KdTexture*> rts);

	// レンダーターゲットを設定
	void SetRenderTarget(uint32_t index, KdTexture* rt);

	// Zバッファを設定
	void SetDepthStencil(const KdTexture* depth);

	// 現在設定されているレンダーターゲットとZバッファををデバイスへセット。
	void SetRenderTargetsAndDepth();

	// rtvの幅・高さを使用して、ビューポートを設定する
	void SetViewportFromRTV(ID3D11RenderTargetView* rtv);

	// 頂点を描画する簡易的な関数
	// ・topology		… 頂点をどのような形状で描画するか　D3D_PRIMITIVE_TOPOLOGYマクロを使用
	// ・vertexCount	… 頂点数
	// ・pVertexStream	… 頂点配列の先頭アドレス
	// ・stride			… １頂点のバイトサイズ
	void DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology, int vertexCount, const void* pVertexStream, UINT stride);


	// (VS)テクスチャを指定スロットにセットする。nullで解除。
	void SetTextureToVS(uint32_t slotNo, KdTexture* tex) const;

	// (PS)テクスチャを指定スロットにセットする。nullで解除。
	void SetTextureToPS(uint32_t slotNo, KdTexture* tex) const;

	// (GS)テクスチャを指定スロットにセットする。nullで解除。
	void SetTextureToGS(uint32_t slotNo, KdTexture* tex) const;

	//==============================================================

	// 
	~KdDirect3D() {
		Release();
	}

private:

	ComPtr<ID3D11Device>		m_pDevice = nullptr;			// Direct3D11の中心になるクラス。全体の管理とバッファ、シェーダ、テクスチャなどのリソース作成などを行う。D3D9とは違って、このクラスは描画関係のメンバ関数を持たない。
	ComPtr<ID3D11DeviceContext> m_pDeviceContext = nullptr;		// 描画処理を行うクラス。
																// 内部的には、レンダリングコマンドと呼ばれるバイナリデータを作成し、GPUに送る。GPUがレンダリングコマンドを解析することで実際の描画処理が行われる。

//	ComPtr<IDXGIAdapter>		m_pGIAdapter = nullptr;			// ディスプレイ サブシステム(1つまたは複数のGPU、DAC、およびビデオ メモリー)。
	ComPtr<IDXGIFactory>		m_pGIFactory = nullptr;			// フルスクリーン切り替えなどで使用。
	ComPtr<IDXGISwapChain>		m_pGISwapChain = nullptr;		// ウインドウへの表示やダブルバッファリングなどを行うクラス。マルチサンプリング、リフレッシュレートなどの設定もできるみたい。

	// バックバッファ＋Zバッファ
	std::shared_ptr<KdRenderTexture>	m_backBuffer;
//	std::shared_ptr<KdRenderTexture> m_rt;

	ComPtr<ID3D11RenderTargetView>		m_setRTVs[8];
	ComPtr<ID3D11DepthStencilView>		m_setDSV;

	// DrawVertices用 頂点バッファ
	std::shared_ptr<KdBuffer>					m_tempFixedVertexBuffer[10];	// 固定長 頂点バッファ
	std::shared_ptr<KdBuffer>					m_tempVertexBuffer;				// 可変長 頂点バッファ

	//------------------------
	// 便利テクスチャ
	//------------------------
	// 1x1 白テクスチャ
	std::shared_ptr<KdTexture>	m_texDotWhite;
	std::shared_ptr<KdTexture>	m_texDotNormal;


//-------------------------------
// シングルトン
//-------------------------------
private:

	KdDirect3D() {
	}

public:
	static KdDirect3D &GetInstance() {
		static KdDirect3D instance;
		return instance;
	}
};

// 簡単にアクセスできるようにするためのマクロ
#define D3D KdDirect3D::GetInstance()
