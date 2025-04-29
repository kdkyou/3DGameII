#include "KdFramework.h"

#include "Editor/KdEditorData.h"

KdFramework* KdFramework::s_instance = nullptr;

void KdFramework::Initialize(void* hWnd)
{
	//===================================================================
	// imgui初期化
	//===================================================================
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(D3D.GetDev().Get(), D3D.GetDevContext().Get());

	#include "imgui/ja_glyph_ranges.h"
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;
	config.MergeMode = true;
	io.Fonts->AddFontDefault();
	// 日本語対応
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);


	//===================================================================
	// 
	// アセットローダーの設定
	// 
	//===================================================================

	// テクスチャロード処理
	KdResourceManager::GetInstance().RegisterAssetLoadFunction(
		"Texture",
		[](const std::string& filename) -> std::shared_ptr<KdObject>
		{
			auto tex = std::make_shared<KdTexture>();
			if (tex->Load(filename) == false)return nullptr;
			return tex;
		}
	);

	// 3Dモデルロード処理
	KdResourceManager::GetInstance().RegisterAssetLoadFunction(
		"Model",
		[](const std::string& filename) -> std::shared_ptr<KdObject>
		{
			auto model = std::make_shared<KdModel>();
			if (model->Load(filename) == false)return nullptr;
			return model;
		}
	);

	// Animationデータ
	KdResourceManager::GetInstance().RegisterAssetLoadFunction(
		"Animation",
		[](const std::string& filename) -> std::shared_ptr<KdObject>
		{
			auto anim = std::make_shared<KdAnimationData>();
			if (anim->Load(filename) == false)return nullptr;
			return anim;
		}
	);

	//===================================================================

	// シーン作成
	m_scene = std::make_shared<KdScene>();
	m_scene->Initialize();

	// レンダリングデータ初期化
	m_renderingData.Initialize();
	//===================================================================


	// エディター関係の初期化
	KdEditorData::GetInstance().Initialize();

	// 時間
	KdTime::GetInstance().Initialize();

}

KdFramework::~KdFramework()
{
	// imgui解放
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void KdFramework::Update()
{
	//=====================================================
	// 
	// 更新処理
	// 
	//=====================================================

	m_renderingData.ResetFrameData();

	m_scene->ResetFrameData();

	// 入力更新
	KdInput::GetInstance().Update();

	// オーディオ処理
	KdAudioManager::GetInstance().Update();

	// エディターの更新処理
	KdEditorData::GetInstance().Update();

	//+++++++++++++++++++++++++++
	// GameObject 更新処理
	//+++++++++++++++++++++++++++

	// 有効なGameObjectを収集する
	m_scene->CollectGameObjects();

	// 収集したGameObjectのUpdateを実行する
	m_scene->Update();

	//=====================================================
	// 
	// 描画処理
	// 
	//=====================================================

	//+++++++++++++++++++++++++++
	// ゲーム画面 カメラ単位の描画(一時テクスチャへ描画)
	//+++++++++++++++++++++++++++

	// 結果テクスチャをクリア
	m_renderingData.m_resultTex->Clear();

	// カメラループ
	for (KdCameraComponent* camera : m_renderingData.m_cameras)
	{
		// 描画先の設定
		m_renderingData.m_currentScreenData = camera->GetScreenData();

		// カメラをセット
		m_renderingData.m_currentScreenData->m_camera = camera;

		// 描画実行(結果テクスチャに出力)
		m_renderingData.m_currentScreenData->Rendering(m_scene->GetCollectedObjects(), m_renderingData.m_resultTex);
	}

	// ゲームモード
	if (KdEditorData::GetInstance().m_editorMode == false)
	{
		// 結果画像をバックバッファへ
		KdShaderManager::GetInstance().m_blitShader.Draw(m_renderingData.m_resultTex.get(), D3D.GetBackBuffer().get());
	}
	// エディターモード
	else
	{
		//+++++++++++++++++++++++++++
		// エディター用 画面描画
		//+++++++++++++++++++++++++++
		// 描画先の設定
		m_renderingData.m_currentScreenData = KdEditorData::GetInstance().m_screenData;

		// カメラをセット
		m_renderingData.m_currentScreenData->m_camera = KdEditorData::GetInstance().m_editorCamera.get();

		// 描画実行(バックバッファに直接出力)
		m_renderingData.m_currentScreenData->Rendering(m_scene->GetCollectedObjects(), D3D.GetBackBuffer());

		//※カメラのゲーム画面は別ウィンドウで表示
	}


	//=====================================================
	// 
	// ImGui処理・描画
	// 
	//=====================================================

	// バックバッファを出力バッファにする
	D3D.SetRenderTargets({ D3D.GetBackBuffer().get() }, nullptr);

	//
	KdEditorData::GetInstance().UpdateImGui();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//=====================================================

	D3D.SetRenderTargets({}, nullptr);

	// BackBuffer -> 画面表示
	D3D.GetSwapChain()->Present(0, 0);

	// 時間更新
	KdTime::GetInstance().Update();

	// 
	KdShaderManager::GetInstance().m_cbSystem->EditCB().Time = KdTime::GetInstance().GetTime();
	KdShaderManager::GetInstance().m_cbSystem->WriteWorkData();
}

