#include "main.h"

#include "Shader/PP_ToneMapping/PP_ToneMapping.h"
#include "Shader/PP_SSAO/PP_SSAO.h"
#include "Shader/PP_Bloom/PP_Bloom.h"

#include "ClassAssembly/KdClassAssembly.h"

//===================================================================
// メイン
//===================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpszArgs, int nWinMode)
{
	// メモリリークを知らせる
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// COM初期化
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// mbstowcs_s関数で日本語対応にするために呼ぶ
	setlocale(LC_ALL, "japanese");

	//DPI変換
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

	//===================================================================
	// 実行
	//===================================================================
	Application::GetInstance().Execute();

	// COM解放
	CoUninitialize();

	return 0;
}


// アプリケーション初期設定
bool Application::Initialize()
{

	int32_t w = 1920;
	int32_t h = 1080;

	//===================================================================
	// ウィンドウ作成
	//===================================================================
	if (m_window.Create(w, h, "KD Framework 2.0", "Window") == false) {
		MessageBoxA(nullptr, "ウィンドウ作成に失敗", "エラー", MB_OK);
		return false;
	}


	//===================================================================
	// フルスクリーン確認
	//===================================================================
//	bool bFullScreen = false;
//	if (MessageBoxA(m_window.GetWndHandle(), "フルスクリーンにしますか？", "確認", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES) {
//		bFullScreen = true;
//	}



	//===================================================================
	// エンジン初期化
	//===================================================================

	// Direct3D初期化
	std::string errorMsg;
	if (D3D.Initialize(m_window.GetWndHandle(), w, h, true, errorMsg) == false) {
		MessageBoxA(m_window.GetWndHandle(), errorMsg.c_str(), "Direct3D初期化失敗", MB_OK | MB_ICONSTOP);
		return false;
	}

	// シェーダーマネージャ初期化
	KdShaderManager::GetInstance().Init();

	// Litシェーダー追加
	{
		auto shader = std::make_shared<KdShader>();

		// 通常描画
		shader->LoadShaderAndSetPass("Assets/Shader/LitShader", "LitShader.hlsl",
			KdShader::PassTags::Forward, "Default",
			"VS", "PS", "");

		// シャドウキャスター用
		shader->LoadShaderAndSetPass("Assets/Shader/LitShader", "LitShader.hlsl",
			KdShader::PassTags::ShadowCaster, "ShadowCaster",
			"ShadowCasterVS", "ShadowCasterPS", "");

		KdShaderManager::GetInstance().AddShader("Lit", shader);
	}

	//Effect用シェーダーの読み込み
	{
		//auto shader = std::make_shared<KdShader>();

		////通常描画
		//shader->LoadShaderAndSetPass(
		//	"Assets/Shader/EffectShader",		//シェーダーまでのパス
		//	"EffectShader.hlsl",				//シェーダーファイル名
		//	KdShader::PassTags::Forward,		//このシェーダーをいつ通す想定か
		//	"Default",							//
		//	"VS", "PS", "");
	}

	// KD Framework 初期化
	KdFramework::CreateInstance();
	KdFramework::GetInstance().Initialize(m_window.GetWndHandle());

	// ポストプロセスシェーダーの追加
	{
		auto pp = std::make_shared<PP_Bloom>();
		pp->Initialize();
		KdRenderSettings::GetInstance().m_postProcesses.push_back(pp);
	}

	{
		auto pp = std::make_shared<PP_SSAO>();
		pp->Initialize();
		KdRenderSettings::GetInstance().m_postProcesses.push_back(pp);
	}

	{
		auto pp = std::make_shared<PP_ToneMapping>();
		pp->Initialize();
		KdRenderSettings::GetInstance().m_postProcesses.push_back(pp);
	}

	//DPI変換
	ImGuiIO& io = ImGui::GetIO();
	UINT dpi = GetDpiForWindow(m_window.GetWndHandle());
	float scale = dpi / 96.0f;
	io.DisplayFramebufferScale = ImVec2(scale, scale);

	//補正
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(m_window.GetWndHandle(), &p);
	io.MousePos = ImVec2(static_cast<float>(p.x), static_cast<float>(p.y));

	return true;
}

// アプリケーション終了
void Application::Release()
{
	// KD Framework 解放
	KdFramework::DeleteInstance();

	// 
	KdShaderManager::GetInstance().Release();

	// Direct3D解放
	D3D.Release();

}


// アプリケーション実行
void Application::Execute()
{
	//===================================================================
	// 初期設定(ウィンドウ作成、Direct3D初期化など)
	//===================================================================
	if (Application::GetInstance().Initialize() == false) {
		return;
	}

	//===================================================================
	// ゲームループ
	//===================================================================

	// ループ
	while (1)
	{
		//		m_fpsControl.Update();

				// 処理開始時間Get
		DWORD st = timeGetTime();



		// ゲーム終了指定があるときはループ終了
		if (m_endFlag)
		{
			break;
		}

		//=========================================
		//
		// ウィンドウ関係の処理
		//
		//=========================================

		// ウィンドウのメッセージを処理する
		m_window.ProcessMessage();

		// ウィンドウが破棄されてるならループ終了
		if (m_window.IsCreated() == false)
		{
			break;
		}

		//=========================================
		// 
		// KD Framework 処理
		// 
		//=========================================
		KdFramework::GetInstance().Update();

	}

	//===================================================================
	// アプリケーション解放
	//===================================================================
	Release();
}
