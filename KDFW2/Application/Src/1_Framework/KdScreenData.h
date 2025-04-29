#pragma once

#include "Component/KdComponent.h"
#include "Component/KdModelRendererComponent.h"
#include "Component/KdCameraComponent.h"

class KdPostProcessBase;

//=========================================================
// 
// １画面ぶんの描画データ
// 
//=========================================================
class KdScreenData
{
public:
	//-------------------------
	// 情報
	//-------------------------
	// 画面の幅
	uint32_t GetScreenWidth() const { return m_color->GetWidth(); }
	// 画面の高さ
	uint32_t GetScreenHeight() const { return m_color->GetHeight(); }
	// 画面のアスペクト比
	float GetScreenAspect() const { return (float)GetScreenWidth() / (float)GetScreenHeight(); }

	// カラーテクスチャ
	const std::shared_ptr<KdRenderTexture>& GetColorTex() { return m_color; }
	const std::shared_ptr<KdRenderTexture>& GetInputColorTex() { return m_inputColor; }
	// Zバッファ
	const std::shared_ptr<KdTexture>& GetDepthTex() { return m_color->GetDepth(); }
	// 画面の法線マップ
	const std::shared_ptr<KdRenderTexture>& GetNormalTex() { return m_normal; }
	// 平行光源用シャドウマップ
	const std::shared_ptr<KdRenderTexture>& GetDirLightShadowMapTex() { return m_dirLightShadowMap; }

	//-------------------------
	// 描画時 一時使用データ
	//-------------------------

	KdCameraComponent* m_camera;

	// 不透明3D描画リスト
	std::vector<KdRendererComponent*>		m_drawList;


	// 描画時一時使用データのリセット
	void ResetTempData()
	{
		m_drawList.clear();
	}

	// 初期設定
	void Initialize()
	{
		// 各レンダーテクスチャの作成
		m_color = std::make_shared<KdRenderTexture>();
		m_color->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, nullptr, DXGI_FORMAT_R24G8_TYPELESS);

		m_normal = std::make_shared<KdRenderTexture>();
		m_normal->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, nullptr, DXGI_FORMAT_UNKNOWN);

		m_inputColor = std::make_shared<KdRenderTexture>();
		m_inputColor->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, nullptr, DXGI_FORMAT_R24G8_TYPELESS);

		m_inputNormal = std::make_shared<KdRenderTexture>();
		m_inputNormal->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, nullptr, DXGI_FORMAT_UNKNOWN);

		m_dirLightShadowMap = std::make_shared<KdRenderTexture>();
		m_dirLightShadowMap->CreateRenderTexture(1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, 1, nullptr, DXGI_FORMAT_R32_TYPELESS);

		ResetTempData();
	}

	void Release()
	{
		m_color = nullptr;
		m_normal = nullptr;
		m_inputColor = nullptr;
		m_inputNormal = nullptr;
	}

	//******************
	// 描画を実行
	//******************
	void Rendering(const std::vector<KdGameObject*>& gameObjects, const std::shared_ptr<KdRenderTexture>& outputTex);

	// ColorTexをInputColorTexにコピーする
	void CopyColorTex()
	{
		m_color->CopyTo(*m_inputColor);
	}
	void CopyNormalTex()
	{
		m_normal->CopyTo(*m_inputNormal);
	}

private:

	// カラーバッファ & Zバッファ
	std::shared_ptr<KdRenderTexture> m_color;
	// 法線
	std::shared_ptr<KdRenderTexture> m_normal;

	// m_colorを入力用にコピーしたもの
	std::shared_ptr<KdRenderTexture> m_inputColor;
	// m_normalを入力用にコピーしたもの
	std::shared_ptr<KdRenderTexture> m_inputNormal;

	// 平行光用シャドウマップ
	std::shared_ptr<KdRenderTexture> m_dirLightShadowMap;
};
