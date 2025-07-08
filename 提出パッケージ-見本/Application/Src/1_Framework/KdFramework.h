#pragma once

#include "CoreSystem.h"

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_PLACEMENT_NEW

#include "imgui/imgui.h"

#include "imgui/imgui_internal.h"

#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_stdlib.h"

#include "imgui/ImGuizmo.h"

// 
#include "KdTime.h"
#include "KdInput.h"

// GameObject、コンポーネント
#include "KdGameObject.h"
#include "KdScene.h"
#include "Component/KdComponent.h"
#include "Component/KdTransformComponent.h"
#include "Component/KdModelRendererComponent.h"
#include "Component/KdCameraComponent.h"
#include "Component/KdLightComponent.h"
#include "Component/KdScriptComponent.h"
#include "Component/KdAnimationComponent.h"

// 
#include "KdScreenData.h"
#include "KdRenderSettings.h"


//=========================================================
// 
// KD Framework
// 
//=========================================================
class KdFramework
{
public:

	// シーン取得
	std::shared_ptr<KdScene> GetScene() { return m_scene; }

	// シーン変更
	void ChangeScene(const std::shared_ptr<KdScene>& scene)
	{
		m_scene = scene;
	}

	void Initialize(void* hWnd);
	~KdFramework();

//	void Release();

	void Update();

private:


public:

	//==================================
	// レンダリング用のデータ
	//==================================
	class RenderingData
	{
	public:

		// 現在の描画先画面への参照
		std::shared_ptr<KdScreenData>		m_currentScreenData;

		// カメラComponent収集用
		std::vector<KdCameraComponent*>		m_cameras;
		// ライトComponent収集用
		std::vector<KdLightComponent*>		m_lights;

		// 最終画像
		std::shared_ptr<KdRenderTexture>	m_resultTex;

		void Initialize()
		{
			m_resultTex = std::make_shared<KdRenderTexture>();
			m_resultTex->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, nullptr, DXGI_FORMAT_UNKNOWN);
		}

		void ResetFrameData()
		{
			m_cameras.clear();
			m_lights.clear();
			m_currentScreenData = nullptr;
		}
	};


	// レンダリングデータ
	RenderingData		m_renderingData;

private:

	// シーン
	std::shared_ptr<KdScene>		m_scene;

//-------------------------------
// シングルトン
//-------------------------------
private:

	static KdFramework* s_instance;

	KdFramework() {
	}

public:
	static void CreateInstance()
	{
		if (s_instance != nullptr)return;
		s_instance = new KdFramework();
	}
	static void DeleteInstance()
	{
		if (s_instance == nullptr)return;
		delete s_instance;
	}
	static KdFramework& GetInstance() {
		return *s_instance;
	}
};
