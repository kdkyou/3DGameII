#pragma once

//#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
// Windows ヘッダー ファイル
#include <windows.h>
// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <objbase.h>	// COM
#include <locale.h>

// ImGui
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_PLACEMENT_NEW

#include "imgui/imgui.h"

#include "imgui/imgui_internal.h"

#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_stdlib.h"

//#include "imgui/ImGuizmo.h"


// ウィンドウ
#include "Windows/WindowsWindow.h"

// 
#include "CoreSystem.h"

#include "KdFramework.h"

//============================================================
// アプリケーションクラス
//	APP.～ でどこからでもアクセス可能
//============================================================
class Application
{
// メンバ
public:

	// アプリケーション実行
	void Execute();

	// アプリケーション終了
	void End() { m_endFlag = true; }


	//=====================================================
	// グローバルデータ
	//=====================================================

	// ゲームウィンドウクラス
	WindowsWindow		m_window;

private:

	// アプリケーション初期化
	bool Initialize();

	// アプリケーション解放
	void Release();

	// ゲーム終了フラグ trueで終了する
	bool		m_endFlag = false;


// シングルトンパターン
private:
	// 
	Application() {}

public:
	static Application &GetInstance(){
		static Application Instance;
		return Instance;
	}
};
