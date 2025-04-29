#include "KdEditorData.h"


bool KdEditorData::OpenFileDialog(std::string& filepath, const std::string& title, const char* filters)
{
	// 現在のカレントディレクトリ保存
	auto current = std::filesystem::current_path();
	// ファイル名のみ
	auto filename = std::filesystem::path(filepath).filename();

	// 結果用
	static char fname[1000];
	strcpy_s(fname, sizeof(fname), filename.string().c_str());

	// デフォルトフォルダ
	std::string dir;
	if (filepath.size() == 0)
	{
		dir = current.string() + "\\";
	}
	else {
		auto path = std::filesystem::absolute(filepath);
		dir = path.parent_path().string() + "\\";
	}

	OPENFILENAMEA o;
	ZeroMemory(&o, sizeof(o));

	o.lStructSize = sizeof(o);									// 構造体サイズ
	o.hwndOwner = nullptr;										// 親ウィンドウのハンドル
	o.lpstrInitialDir = dir.c_str();							// 初期フォルダー
	o.lpstrFile = fname;										// 取得したファイル名を保存するバッファ
	o.nMaxFile = sizeof(fname);									// 取得したファイル名を保存するバッファサイズ
	o.lpstrFilter = filters;									// (例) "TXTファイル(*.TXT)\0*.TXT\0全てのファイル(*.*)\0*.*\0";
	o.lpstrDefExt = "";
	o.lpstrTitle = title.c_str();
	o.nFilterIndex = 1;
	if (GetOpenFileNameA(&o))
	{
		// カレントディレクトリを元に戻す
		std::filesystem::current_path(current);
		// 相対パスへ変換
		filepath = std::filesystem::relative(fname).string();
		return true;
	}
	std::filesystem::current_path(current);	// カレントディレクトリを元に戻す
	return false;
}

bool KdEditorData::SaveFileDialog(std::string& filepath, const std::string& title, const char* filters, const std::string& defExt)
{
	// 現在のカレントディレクトリ保存
	auto current = std::filesystem::current_path();
	// ファイル名のみ
	auto filename = std::filesystem::path(filepath).filename();

	// 結果用
	static char fname[1000];
	strcpy_s(fname, sizeof(fname), filename.string().c_str());

	// デフォルトフォルダ
	std::string dir;
	if (filepath.size() == 0)
	{
		dir = current.string() + "\\";
	}
	else {
		auto path = std::filesystem::absolute(filepath);
		dir = path.parent_path().string() + "\\";
	}

	OPENFILENAMEA o;
	ZeroMemory(&o, sizeof(o));

	o.lStructSize = sizeof(o);									// 構造体サイズ
	o.hwndOwner = nullptr;										// 親ウィンドウのハンドル
	o.lpstrInitialDir = dir.c_str();						// 初期フォルダー
	o.lpstrFile = fname;										// 取得したファイル名を保存するバッファ
	o.nMaxFile = sizeof(fname);									// 取得したファイル名を保存するバッファサイズ
	o.lpstrFilter = filters;									// (例) "TXTファイル(*.TXT)\0*.TXT\0全てのファイル(*.*)\0*.*\0";
	o.lpstrDefExt = defExt.c_str();
	o.lpstrTitle = title.c_str();
	o.nFilterIndex = 1;
	o.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (GetSaveFileNameA(&o))
	{
		// カレントディレクトリを元に戻す
		std::filesystem::current_path(current);
		// 相対パスへ変換
		filepath = std::filesystem::relative(fname).string();
		return true;
	}
	std::filesystem::current_path(current);	// カレントディレクトリを元に戻す
	return false;
}


// 

void KdEditorData::Initialize()
{
	m_cameraObject = std::make_shared<KdGameObject>();
	m_cameraObject->Initialize("EditorCamera");

	m_editorCamera = m_cameraObject->AddComponent<KdCameraComponent>();

	auto pos = m_cameraObject->GetTransform()->GetLocalPosition();
	pos.z = -5;
	m_cameraObject->GetTransform()->SetLocalPosition(pos);
	// 
	m_screenData = std::make_shared<KdScreenData>();
	m_screenData->Initialize();
}

void KdEditorData::Update()
{
	// 
	if (KdInput::GetInstance().GetKeyboardTracker().pressed.F5)
	{
		m_editorMode = !m_editorMode;
	}


	{
		auto mouseTracker = KdInput::GetInstance().GetMouseTracker();
		auto mouseState = KdInput::GetInstance().GetMouseState();
		auto keyState = KdInput::GetInstance().GetKeyboardState();

		//============================
		// ビュー行列
		//============================
		POINT nowPos;
		GetCursorPos(&nowPos);

		// Shiftキーで比率を大きく
		float ratio = 1.0f;
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			ratio = 5;
		}
		// Ctrlキーで比率を小さく
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			ratio = 0.2f;
		}

		// カメラの現在位置
		auto pos = m_cameraObject->GetTransform()->GetPosition();

		// カメラの姿勢
		KdMatrix wCamMat = m_cameraObject->GetTransform()->GetWorldMatrix();
		wCamMat.Translation(KdVector3(0, 0, 0));

		// マウスの右ボタン
		if (mouseState.rightButton && mouseState.positionMode == DirectX::Mouse::MODE_RELATIVE)
		{
			wCamMat *= KdMatrix::CreateRotationY(mouseState.x * 0.3f * KdDeg2Rad);
			wCamMat *= KdMatrix::CreateFromAxisAngle(wCamMat.Right(), mouseState.y * 0.3f * KdDeg2Rad);

			if (keyState.W)
			{
				pos += wCamMat.Backward() * 0.1f;
			}
			if (keyState.S)
			{
				pos -= wCamMat.Backward() * 0.1f;
			}
			if (keyState.A)
			{
				pos -= wCamMat.Right() * 0.1f;
			}
			if (keyState.D)
			{
				pos += wCamMat.Right() * 0.1f;
			}
			if (keyState.E)
			{
				pos += wCamMat.Up() * 0.1f;
			}
			if (keyState.Q)
			{
				pos -= wCamMat.Up() * 0.1f;
			}
		}



		// マウスの中ボタン
		if(mouseState.middleButton && mouseState.positionMode == DirectX::Mouse::MODE_RELATIVE)
		{
			static const float moveRatio = 0.01f;
			KdVector3 vMove = {};
			vMove += -(float)mouseState.y * wCamMat.Up() * ratio * moveRatio;
			vMove += -(float)mouseState.x * wCamMat.Right() * ratio * moveRatio;
			pos += vMove;
		}

		// カメラ更新
		wCamMat.Translation(pos);
		m_cameraObject->GetTransform()->SetWorldMatrix(wCamMat);

		// マウスの動作モード変更
		if (mouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED || 
			mouseTracker.middleButton == DirectX::Mouse::ButtonStateTracker::ButtonState::PRESSED)
		{
			KdInput::GetInstance().GetMouse().SetMode(DirectX::Mouse::MODE_RELATIVE);
		}
		else if(mouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED || 
				mouseTracker.middleButton == DirectX::Mouse::ButtonStateTracker::ButtonState::RELEASED)
		{
			KdInput::GetInstance().GetMouse().SetMode(DirectX::Mouse::MODE_ABSOLUTE);
		}
	}
}

void KdEditorData::UpdateImGui()
{
	//===========================
	// ImGui開始
	//===========================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	// ImGui Demo ウィンドウ表示 ※すごく参考になるウィンドウです。imgui_demo.cpp参照。
	ImGui::ShowDemoWindow(nullptr);


	//===========================
	// システム
	//===========================
	if (ImGui::Begin("System", 0, ImGuiWindowFlags_MenuBar))
	{
		// メニュー 
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(u8"ファイル"))
			{

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Checkbox(u8"エディターモード", &m_editorMode);
		/*
		if (m_editorMode)
		{
			ImGui::Text(u8"エディターモード");
		}
		else
		{
			ImGui::Text(u8"ゲームモード");
		}
		*/

		ImGui::LabelText("FPS", "%f", 1.0f / KdTime::GetInstance().GetDeltaTime());

		// ImGuizmo
		if (ImGui::CollapsingHeader("Gizmo", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (ImGui::RadioButton("Local", GizmoMode == ImGuizmo::LOCAL)) {
				GizmoMode = ImGuizmo::LOCAL;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton("World", GizmoMode == ImGuizmo::WORLD)) {
				GizmoMode = ImGuizmo::WORLD;
			}

			if (ImGui::RadioButton(u8"移動", GizmoOperation == ImGuizmo::TRANSLATE)) {
				GizmoOperation = ImGuizmo::TRANSLATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton(u8"回転", GizmoOperation == ImGuizmo::ROTATE)) {
				GizmoOperation = ImGuizmo::ROTATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton(u8"拡大", GizmoOperation == ImGuizmo::SCALE)) {
				GizmoOperation = ImGuizmo::SCALE;
			}
		}
	}
	ImGui::End();

	//===========================
	// Inspectorウィンドウ
	//===========================
	if (ImGui::Begin("Inspector"))
	{
		auto nowSelectedObj = SelectObj.lock();
		if (nowSelectedObj != nullptr)
		{
			nowSelectedObj->Editor_ImGui();

			ImGui::Separator();

			if (ImGui::Button(u8"コンポーネント追加"))
			{

			}
			if (ImGui::BeginPopupContextItem("AddComponentPopup", 0))
			{
				auto list = *KdClassAssembly::s_assemblies;

				for (auto&& n : list)
				{
					if (n.second.Tag != "Component")continue;

					if (ImGui::Selectable(n.first.c_str()))
					{
						nowSelectedObj->AddComponent(n.first);
					}
				}

				ImGui::EndPopup();
			}
		}

	}
	ImGui::End();

	//===========================
	// Hierarchyウィンドウ
	//===========================
	KdFramework::GetInstance().GetScene()->Editor_ImGui();

	//===========================
	// ログウィンドウ
	//===========================
	m_logWindow.ImGuiUpdate("Log Window");

	//===========================
	// GameViewウィンドウ
	//===========================
	// エディターモード時は、ゲーム画面を表示
	if (KdEditorData::GetInstance().m_editorMode)
	{
		if (ImGui::Begin("Game View", 0, 0))
		{
			auto tex = KdFramework::GetInstance().m_renderingData.m_resultTex;

			ImVec2 wSize = ImGui::GetWindowSize();

			wSize.y = wSize.x / tex->GetAspectRatio();
			ImGui::Image((ImTextureID)tex->GetSRView().Get(), { wSize.x, wSize.y });
		}
		ImGui::End();
	}

	//===========================
	// シェーダーウィンドウ
	//===========================
	if (ImGui::Begin("Render Settings", 0, 0))
	{
		// 
		KdRenderSettings::GetInstance().Editor_ImGui();
	}
	ImGui::End();

}
