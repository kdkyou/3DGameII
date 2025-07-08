#include "WindowsWindow.h"

#include "resource.h"

#include "../main.h"


// imgui
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool WindowsWindow::Create(int clientWidth, int clientHeight, const std::string& titleName, const std::string& windowClassName)
{
	// インスタンスハンドル取得
	HINSTANCE hInst = GetInstanceHandle();
	
	//===================================================================
	// メインウィンドウ作成
	//===================================================================

	std::wstring wndClsName = L"KDFW2App";// PWE::sjis_to_wide(windowClassName);

	//ウィンドウクラスの定義
	WNDCLASSEX wc;											// ウィンドウクラスの定義用
	wc.cbSize = sizeof(WNDCLASSEX);							// 構造体のサイズ
	wc.style = 0;											// スタイル
	wc.lpfnWndProc = &WindowsWindow::callWindowProc;			// ウインドウ関数
	wc.cbClsExtra = 0;										// エキストラクラス情報 
	wc.cbWndExtra = 0;										// エキストラウィンドウ情報
	wc.hInstance = hInst;									// インスタンスハンドル
	wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPLICATION));			// ラージアイコン
	wc.hIconSm = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));		// スモールアイコン 
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);			// マウスカーソル
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);	// 背景色 
	wc.lpszMenuName = nullptr;								// メインメニュー名
	wc.lpszClassName = wndClsName.c_str();					// ウィンドウクラス名

	//ウィンドウクラスの登録
	if (!RegisterClassEx(&wc)) {
		return false;
	}

	//ウィンドウの作成
	/*
	m_hWnd = CreateWindow(
		wndClsName.c_str(),									// ウィンドウクラス名
		sjis_to_wide(titleName).c_str(),					// ウィンドウのタイトル
		WS_POPUP,											// ウィンドウタイプを標準タイプに	
		0,													// ウィンドウの位置（Ｘ座標）
		0,													// ウィンドウの位置（Ｙ座標）						
		clientWidth,										// ウィンドウの幅
		clientHeight,										// ウィンドウの高さ			
		nullptr,											// 親ウィンドウのハンドル
		nullptr,											// メニューのハンドル
		hInst,												// インスタンスハンドル 
		this);												// 追加情報
	*/
	m_hWnd = CreateWindow(
		wndClsName.c_str(),									// ウィンドウクラス名
		sjis_to_wide(titleName).c_str(),					// ウィンドウのタイトル
		WS_OVERLAPPEDWINDOW,								// ウィンドウタイプを標準タイプに	
		0,													// ウィンドウの位置（Ｘ座標）
		0,													// ウィンドウの位置（Ｙ座標）						
		clientWidth,										// ウィンドウの幅
		clientHeight,										// ウィンドウの高さ			
		nullptr,											// 親ウィンドウのハンドル
		nullptr,											// メニューのハンドル
		hInst,												// インスタンスハンドル 
		this);												// 追加情報

	if (m_hWnd == nullptr) {
		return false;
	}

	// クライアントのサイズを設定
	SetClientSize(clientWidth, clientHeight);
	
	//===================================================================
	//ウィンドウの表示
	//===================================================================
	ShowWindow(m_hWnd, SW_SHOW);
	//ウィンドウの更新
	UpdateWindow(m_hWnd);

	// timeGetTime関数の精度を1msに設定
	timeBeginPeriod(1);

	// KDFramework2.0
	KdInput::GetInstance().SetWindow(m_hWnd);


	return true;
}

void WindowsWindow::Release()
{
	if (m_hWnd) {
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

bool WindowsWindow::ProcessMessage()
{
	// メッセージ取得
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// 終了メッセージがきた
		if (msg.message == WM_QUIT) {
			return false;
		}

		//メッセージ処理
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

// ウィンドウ関数(Static関数)
LRESULT CALLBACK WindowsWindow::callWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ウィンドウプロパティから、GameWindowクラスのインスタンスを取得
	WindowsWindow* pThis = (WindowsWindow*)GetProp(hWnd, L"GameWindowInstance");

	// nullptrの場合は、デフォルト処理を実行
	if (pThis == nullptr) {
		switch (message) {
			case WM_CREATE:
			{
				// CreateWindow()で渡したパラメータを取得
				CREATESTRUCT * createStruct = (CREATESTRUCT*)lParam;
				WindowsWindow* window = (WindowsWindow*)createStruct->lpCreateParams;

				// ウィンドウプロパティにこのクラスのインスタンスアドレスを埋め込んでおく
				// 次回から、pThis->WindowProcの方へ処理が流れていく
				SetProp(hWnd, L"GameWindowInstance", window);

			}
			return 0;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	// インスタンス側のWindow関数を実行する
	return pThis->WindowProc(hWnd, message, wParam, lParam);
}

// ウィンドウ関数
LRESULT WindowsWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// ImGuiにイベント通知
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) {
		return true;
	}

	// KDFramework2.0
	KdInput::GetInstance().ProcessMessage(message, wParam, lParam);

	//===================================
	//メッセージによって処理を選択
	//===================================
	switch (message) {
	// Xボタンが押された
	case WM_CLOSE:
		// 破棄
		Release();
		break;
	// ウィンドウ破棄直前
	case WM_DESTROY:
		RemoveProp(hWnd, L"GameWindowInstance");
		PostQuitMessage(0);
		break;
	}

	// メッセージのデフォルト処理
	return DefWindowProc(hWnd, message, wParam, lParam);
}

// ウィンドウのクライアントのサイズを指定サイズにする。
void WindowsWindow::SetClientSize(int w, int h)
{
	RECT rcWnd, rcCli;

	GetWindowRect(m_hWnd, &rcWnd); // ウィンドウのRECT取得
	GetClientRect(m_hWnd, &rcCli); // クライアント領域のRECT取得

	// ウィンドウの余白を考えて、クライアントのサイズを指定サイズにする。
	MoveWindow(m_hWnd,
		rcWnd.left,// X座標
		rcWnd.top,// Y座標
		w + (rcWnd.right - rcWnd.left) - (rcCli.right - rcCli.left),
		h + (rcWnd.bottom - rcWnd.top) - (rcCli.bottom - rcCli.top),
		TRUE);
}
