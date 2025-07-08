#pragma once

#include "KdFramework.h"
#include "ImGuiHelper.h"

class KdEditorData
{
public:
	// 選択GameObject
	std::weak_ptr<KdGameObject>		SelectObj;

	uint32_t	m_ScreenW = 1920;
	uint32_t	m_screenH = 1080;

	bool		m_editorMode = true;

	// エディターカメラ
	std::shared_ptr<KdGameObject>			m_cameraObject;
	std::shared_ptr<KdCameraComponent>		m_editorCamera;

	// ImGuizmo
	ImGuizmo::MODE			GizmoMode = ImGuizmo::LOCAL;
	ImGuizmo::OPERATION		GizmoOperation = ImGuizmo::TRANSLATE;

	// 画面データ
	std::shared_ptr<KdScreenData> m_screenData;

	// ログウィンドウ
	ImGuiLogWindow			m_logWindow;

	// ファイルを開くダイアログボックスを表示
	// ・filepath		… 選択されたファイルパスが入る
	// ・title			… ウィンドウのタイトル文字
	// ・filters		… 指定された拡張子のみ表示されるようになる
	static bool OpenFileDialog(std::string& filepath, const std::string& title = "ファイルを開く", const char* filters = "全てのファイル\0*.*\0");
	// ファイル名をつけて保存ダイアログボックスを表示
	// ・filepath		… 選択されたファイルパスが入る
	// ・title			… ウィンドウのタイトル文字
	// ・filters		… 指定された拡張子のみ表示されるようになる
	// ・defExt			… ユーザーが拡張子を入力しなかった場合、これが自動的に付く
	static bool SaveFileDialog(std::string& filepath, const std::string& title = "ファイルを保存", const char* filters = "全てのファイル\0*.*\0", const std::string& defExt = "");

	// 
	void Initialize();

	void Update();

	// 
	void UpdateImGui();

//-------------------------------
// シングルトン
//-------------------------------
private:

	KdEditorData()
	{
	}

public:
	static KdEditorData& GetInstance() {
		static KdEditorData instance;
		return instance;
	}
};

