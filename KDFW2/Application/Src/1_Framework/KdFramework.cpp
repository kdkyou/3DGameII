#include "KdFramework.h"

#include "Editor/KdEditorData.h"

KdFramework* KdFramework::s_instance = nullptr;

void KdFramework::Initialize(void* hWnd)
{
	//===================================================================
	// imgui������
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
	// ���{��Ή�
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msgothic.ttc", 13.0f, &config, glyphRangesJapanese);


	//===================================================================
	// 
	// �A�Z�b�g���[�_�[�̐ݒ�
	// 
	//===================================================================

	// �e�N�X�`�����[�h����
	KdResourceManager::GetInstance().RegisterAssetLoadFunction(
		"Texture",
		[](const std::string& filename) -> std::shared_ptr<KdObject>
		{
			auto tex = std::make_shared<KdTexture>();
			if (tex->Load(filename) == false)return nullptr;
			return tex;
		}
	);

	// 3D���f�����[�h����
	KdResourceManager::GetInstance().RegisterAssetLoadFunction(
		"Model",
		[](const std::string& filename) -> std::shared_ptr<KdObject>
		{
			auto model = std::make_shared<KdModel>();
			if (model->Load(filename) == false)return nullptr;
			return model;
		}
	);

	// Animation�f�[�^
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

	// �V�[���쐬
	m_scene = std::make_shared<KdScene>();
	m_scene->Initialize();

	// �����_�����O�f�[�^������
	m_renderingData.Initialize();
	//===================================================================


	// �G�f�B�^�[�֌W�̏�����
	KdEditorData::GetInstance().Initialize();

	// ����
	KdTime::GetInstance().Initialize();

}

KdFramework::~KdFramework()
{
	// imgui���
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}

void KdFramework::Update()
{
	//=====================================================
	// 
	// �X�V����
	// 
	//=====================================================

	m_renderingData.ResetFrameData();

	m_scene->ResetFrameData();

	// ���͍X�V
	KdInput::GetInstance().Update();

	// �I�[�f�B�I����
	KdAudioManager::GetInstance().Update();

	// �G�f�B�^�[�̍X�V����
	KdEditorData::GetInstance().Update();

	//+++++++++++++++++++++++++++
	// GameObject �X�V����
	//+++++++++++++++++++++++++++

	// �L����GameObject�����W����
	m_scene->CollectGameObjects();

	// ���W����GameObject��Update�����s����
	m_scene->Update();

	//=====================================================
	// 
	// �`�揈��
	// 
	//=====================================================

	//+++++++++++++++++++++++++++
	// �Q�[����� �J�����P�ʂ̕`��(�ꎞ�e�N�X�`���֕`��)
	//+++++++++++++++++++++++++++

	// ���ʃe�N�X�`�����N���A
	m_renderingData.m_resultTex->Clear();

	// �J�������[�v
	for (KdCameraComponent* camera : m_renderingData.m_cameras)
	{
		// �`���̐ݒ�
		m_renderingData.m_currentScreenData = camera->GetScreenData();

		// �J�������Z�b�g
		m_renderingData.m_currentScreenData->m_camera = camera;

		// �`����s(���ʃe�N�X�`���ɏo��)
		m_renderingData.m_currentScreenData->Rendering(m_scene->GetCollectedObjects(), m_renderingData.m_resultTex);
	}

	// �Q�[�����[�h
	if (KdEditorData::GetInstance().m_editorMode == false)
	{
		// ���ʉ摜���o�b�N�o�b�t�@��
		KdShaderManager::GetInstance().m_blitShader.Draw(m_renderingData.m_resultTex.get(), D3D.GetBackBuffer().get());
	}
	// �G�f�B�^�[���[�h
	else
	{
		//+++++++++++++++++++++++++++
		// �G�f�B�^�[�p ��ʕ`��
		//+++++++++++++++++++++++++++
		// �`���̐ݒ�
		m_renderingData.m_currentScreenData = KdEditorData::GetInstance().m_screenData;

		// �J�������Z�b�g
		m_renderingData.m_currentScreenData->m_camera = KdEditorData::GetInstance().m_editorCamera.get();

		// �`����s(�o�b�N�o�b�t�@�ɒ��ڏo��)
		m_renderingData.m_currentScreenData->Rendering(m_scene->GetCollectedObjects(), D3D.GetBackBuffer());

		//���J�����̃Q�[����ʂ͕ʃE�B���h�E�ŕ\��
	}


	//=====================================================
	// 
	// ImGui�����E�`��
	// 
	//=====================================================

	// �o�b�N�o�b�t�@���o�̓o�b�t�@�ɂ���
	D3D.SetRenderTargets({ D3D.GetBackBuffer().get() }, nullptr);

	//
	KdEditorData::GetInstance().UpdateImGui();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//=====================================================

	D3D.SetRenderTargets({}, nullptr);

	// BackBuffer -> ��ʕ\��
	D3D.GetSwapChain()->Present(0, 0);

	// ���ԍX�V
	KdTime::GetInstance().Update();

	// 
	KdShaderManager::GetInstance().m_cbSystem->EditCB().Time = KdTime::GetInstance().GetTime();
	KdShaderManager::GetInstance().m_cbSystem->WriteWorkData();
}

