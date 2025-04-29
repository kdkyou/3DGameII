#include "KdEditorData.h"


bool KdEditorData::OpenFileDialog(std::string& filepath, const std::string& title, const char* filters)
{
	// ���݂̃J�����g�f�B���N�g���ۑ�
	auto current = std::filesystem::current_path();
	// �t�@�C�����̂�
	auto filename = std::filesystem::path(filepath).filename();

	// ���ʗp
	static char fname[1000];
	strcpy_s(fname, sizeof(fname), filename.string().c_str());

	// �f�t�H���g�t�H���_
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

	o.lStructSize = sizeof(o);									// �\���̃T�C�Y
	o.hwndOwner = nullptr;										// �e�E�B���h�E�̃n���h��
	o.lpstrInitialDir = dir.c_str();							// �����t�H���_�[
	o.lpstrFile = fname;										// �擾�����t�@�C������ۑ�����o�b�t�@
	o.nMaxFile = sizeof(fname);									// �擾�����t�@�C������ۑ�����o�b�t�@�T�C�Y
	o.lpstrFilter = filters;									// (��) "TXT�t�@�C��(*.TXT)\0*.TXT\0�S�Ẵt�@�C��(*.*)\0*.*\0";
	o.lpstrDefExt = "";
	o.lpstrTitle = title.c_str();
	o.nFilterIndex = 1;
	if (GetOpenFileNameA(&o))
	{
		// �J�����g�f�B���N�g�������ɖ߂�
		std::filesystem::current_path(current);
		// ���΃p�X�֕ϊ�
		filepath = std::filesystem::relative(fname).string();
		return true;
	}
	std::filesystem::current_path(current);	// �J�����g�f�B���N�g�������ɖ߂�
	return false;
}

bool KdEditorData::SaveFileDialog(std::string& filepath, const std::string& title, const char* filters, const std::string& defExt)
{
	// ���݂̃J�����g�f�B���N�g���ۑ�
	auto current = std::filesystem::current_path();
	// �t�@�C�����̂�
	auto filename = std::filesystem::path(filepath).filename();

	// ���ʗp
	static char fname[1000];
	strcpy_s(fname, sizeof(fname), filename.string().c_str());

	// �f�t�H���g�t�H���_
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

	o.lStructSize = sizeof(o);									// �\���̃T�C�Y
	o.hwndOwner = nullptr;										// �e�E�B���h�E�̃n���h��
	o.lpstrInitialDir = dir.c_str();						// �����t�H���_�[
	o.lpstrFile = fname;										// �擾�����t�@�C������ۑ�����o�b�t�@
	o.nMaxFile = sizeof(fname);									// �擾�����t�@�C������ۑ�����o�b�t�@�T�C�Y
	o.lpstrFilter = filters;									// (��) "TXT�t�@�C��(*.TXT)\0*.TXT\0�S�Ẵt�@�C��(*.*)\0*.*\0";
	o.lpstrDefExt = defExt.c_str();
	o.lpstrTitle = title.c_str();
	o.nFilterIndex = 1;
	o.Flags = OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (GetSaveFileNameA(&o))
	{
		// �J�����g�f�B���N�g�������ɖ߂�
		std::filesystem::current_path(current);
		// ���΃p�X�֕ϊ�
		filepath = std::filesystem::relative(fname).string();
		return true;
	}
	std::filesystem::current_path(current);	// �J�����g�f�B���N�g�������ɖ߂�
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
		// �r���[�s��
		//============================
		POINT nowPos;
		GetCursorPos(&nowPos);

		// Shift�L�[�Ŕ䗦��傫��
		float ratio = 1.0f;
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
		{
			ratio = 5;
		}
		// Ctrl�L�[�Ŕ䗦��������
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		{
			ratio = 0.2f;
		}

		// �J�����̌��݈ʒu
		auto pos = m_cameraObject->GetTransform()->GetPosition();

		// �J�����̎p��
		KdMatrix wCamMat = m_cameraObject->GetTransform()->GetWorldMatrix();
		wCamMat.Translation(KdVector3(0, 0, 0));

		// �}�E�X�̉E�{�^��
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



		// �}�E�X�̒��{�^��
		if(mouseState.middleButton && mouseState.positionMode == DirectX::Mouse::MODE_RELATIVE)
		{
			static const float moveRatio = 0.01f;
			KdVector3 vMove = {};
			vMove += -(float)mouseState.y * wCamMat.Up() * ratio * moveRatio;
			vMove += -(float)mouseState.x * wCamMat.Right() * ratio * moveRatio;
			pos += vMove;
		}

		// �J�����X�V
		wCamMat.Translation(pos);
		m_cameraObject->GetTransform()->SetWorldMatrix(wCamMat);

		// �}�E�X�̓��샂�[�h�ύX
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
	// ImGui�J�n
	//===========================
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	// ImGui Demo �E�B���h�E�\�� ���������Q�l�ɂȂ�E�B���h�E�ł��Bimgui_demo.cpp�Q�ƁB
	ImGui::ShowDemoWindow(nullptr);


	//===========================
	// �V�X�e��
	//===========================
	if (ImGui::Begin("System", 0, ImGuiWindowFlags_MenuBar))
	{
		// ���j���[ 
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(u8"�t�@�C��"))
			{

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Checkbox(u8"�G�f�B�^�[���[�h", &m_editorMode);
		/*
		if (m_editorMode)
		{
			ImGui::Text(u8"�G�f�B�^�[���[�h");
		}
		else
		{
			ImGui::Text(u8"�Q�[�����[�h");
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

			if (ImGui::RadioButton(u8"�ړ�", GizmoOperation == ImGuizmo::TRANSLATE)) {
				GizmoOperation = ImGuizmo::TRANSLATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton(u8"��]", GizmoOperation == ImGuizmo::ROTATE)) {
				GizmoOperation = ImGuizmo::ROTATE;
			}
			ImGui::SameLine();
			if (ImGui::RadioButton(u8"�g��", GizmoOperation == ImGuizmo::SCALE)) {
				GizmoOperation = ImGuizmo::SCALE;
			}
		}
	}
	ImGui::End();

	//===========================
	// Inspector�E�B���h�E
	//===========================
	if (ImGui::Begin("Inspector"))
	{
		auto nowSelectedObj = SelectObj.lock();
		if (nowSelectedObj != nullptr)
		{
			nowSelectedObj->Editor_ImGui();

			ImGui::Separator();

			if (ImGui::Button(u8"�R���|�[�l���g�ǉ�"))
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
	// Hierarchy�E�B���h�E
	//===========================
	KdFramework::GetInstance().GetScene()->Editor_ImGui();

	//===========================
	// ���O�E�B���h�E
	//===========================
	m_logWindow.ImGuiUpdate("Log Window");

	//===========================
	// GameView�E�B���h�E
	//===========================
	// �G�f�B�^�[���[�h���́A�Q�[����ʂ�\��
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
	// �V�F�[�_�[�E�B���h�E
	//===========================
	if (ImGui::Begin("Render Settings", 0, 0))
	{
		// 
		KdRenderSettings::GetInstance().Editor_ImGui();
	}
	ImGui::End();

}
