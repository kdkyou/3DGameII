#pragma once

#include "KdFramework.h"
#include "ImGuiHelper.h"

class KdEditorData
{
public:
	// �I��GameObject
	std::weak_ptr<KdGameObject>		SelectObj;

	uint32_t	m_ScreenW = 1920;
	uint32_t	m_screenH = 1080;

	bool		m_editorMode = true;

	// �G�f�B�^�[�J����
	std::shared_ptr<KdGameObject>			m_cameraObject;
	std::shared_ptr<KdCameraComponent>		m_editorCamera;

	// ImGuizmo
	ImGuizmo::MODE			GizmoMode = ImGuizmo::LOCAL;
	ImGuizmo::OPERATION		GizmoOperation = ImGuizmo::TRANSLATE;

	// ��ʃf�[�^
	std::shared_ptr<KdScreenData> m_screenData;

	// ���O�E�B���h�E
	ImGuiLogWindow			m_logWindow;

	// �t�@�C�����J���_�C�A���O�{�b�N�X��\��
	// �Efilepath		�c �I�����ꂽ�t�@�C���p�X������
	// �Etitle			�c �E�B���h�E�̃^�C�g������
	// �Efilters		�c �w�肳�ꂽ�g���q�̂ݕ\�������悤�ɂȂ�
	static bool OpenFileDialog(std::string& filepath, const std::string& title = "�t�@�C�����J��", const char* filters = "�S�Ẵt�@�C��\0*.*\0");
	// �t�@�C���������ĕۑ��_�C�A���O�{�b�N�X��\��
	// �Efilepath		�c �I�����ꂽ�t�@�C���p�X������
	// �Etitle			�c �E�B���h�E�̃^�C�g������
	// �Efilters		�c �w�肳�ꂽ�g���q�̂ݕ\�������悤�ɂȂ�
	// �EdefExt			�c ���[�U�[���g���q����͂��Ȃ������ꍇ�A���ꂪ�����I�ɕt��
	static bool SaveFileDialog(std::string& filepath, const std::string& title = "�t�@�C����ۑ�", const char* filters = "�S�Ẵt�@�C��\0*.*\0", const std::string& defExt = "");

	// 
	void Initialize();

	void Update();

	// 
	void UpdateImGui();

//-------------------------------
// �V���O���g��
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

