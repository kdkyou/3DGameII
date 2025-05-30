#include"KdEffectComponent.h"

//KdPolygon�̒��ginclude
#include"0_App/Shader/Polygon/KdPolygon.h"
//�`�悷��̂ɕK�v
#include"../KdFramework.h"
//�A�Z�b�g�֌W�ɕK�v
#include"Editor/KdEditorData.h"

SetClassAssembly(KdEffectComponent, "Component");

void KdEffectComponent::Start()
{
	//�|���S���쐬
	m_poly = std::make_shared<KdPolygon>();
	m_poly->Initialize();

	//�|���S���̌`���쐬	(1m*1m�Œ�ύX����scale��������)
	m_poly->AddVertex(KdVector3(-0.5f, -0.5f, 0), KdVector2(0.0f, 1.0f), 0xFFFFFFFF);		//����
	m_poly->AddVertex(KdVector3(-0.5f, 0.5f, 0), KdVector2(0.0f, 0.0f), 0xFFFFFFFF);		//����
	m_poly->AddVertex(KdVector3(0.5f, -0.5f, 0), KdVector2(1.0f, 1.0f), 0xFFFFFFFF);		//�E��
	m_poly->AddVertex(KdVector3(0.5f, 0.5f, 0), KdVector2(1.0f, 0.0f), 0xFFFFFFFF);		//�E��

	
	if (m_playWithStart == true)
	{
		StartAnim();
	}

}

void KdEffectComponent::Update()
{

	if (m_isStarted == false) { return; }

	m_scale = KdVector2::Lerp(m_startScale, m_endScale, m_duration);

	float time =m_ease.NextValue();

	m_duration = time;

	if (m_duration > m_lifetime) {
		m_duration = m_lifetime;
		m_isStarted = false;
	}

}

void KdEffectComponent::LateUpdate()
{

}


void KdEffectComponent::PreDraw()
{
	//���̃I�u�W�F�N�g���L����
	if (IsEnable() == false) { return; }

	//���݁ADraw�O��i��̂ݔ��肵�Ă��邪�ADraw�̓J�����񐔕����

	//�r���{�[�h����
	
	// ����GameObject��Transform
	const auto& trans = GetGameObject()->GetTransform();

	// �J�����̏��
	auto camera = KdFramework::GetInstance().m_renderingData.m_currentScreenData->m_camera;

	//�J�����̍s��
	auto camMat = camera->GetCameraMatrix();
	//��]���������𒊏o
	camMat._41 = camMat._42 = camMat._43 = 0;

	// GameObject�̈ʒu���
	auto pos = trans->GetLocalPosition();
	KdMatrix posMat = KdMatrix::CreateTranslation(pos);

	//�ǉ���Z��]�p�x
	KdMatrix zRotMat = KdMatrix::CreateRotationZ(m_zRotation * KdDeg2Rad);

	//�ǉ��̃X�P�[��
	KdMatrix scaleMat = KdMatrix::CreateScale(m_scale.x,m_scale.y,1.0f);

	//�s�񍇐�
	KdMatrix totalMat = scaleMat* zRotMat * camMat  * posMat;

	trans->SetLocalMatrix(totalMat);



	//�`��ΏۂƂ��ēo�^
	KdFramework::GetInstance().m_renderingData.m_currentScreenData->m_drawList.push_back(this);



}

void KdEffectComponent::Draw(bool opaque, KdShader::PassTags passTag)
{
	if (m_poly == nullptr) { return; }	//�`��Ώۂ����Ȃ�

	// �s�������`��Ŗ�����(�������ł���)
	if (opaque == false) { return; }

	//�e�v�`��̎�
	if (passTag == KdShader::PassTags::ShadowCaster) { return; }
	
	//�V�F�[�_�[
	auto& sm = KdShaderManager::GetInstance();

	// GameObject�̏ꏊ�ɕ`��
	sm.m_cbPerDraw->EditCB().mW = GetGameObject()->GetTransform()->GetWorldMatrix();

	sm.m_cbPerDraw->WriteWorkData();	//�V�F�[�_�[�ɑ��荞��

	// ���Z�����ɂ���
	sm.m_bs_Add->SetToDevice();

	//�`��
	m_poly->Draw(KdPolygon::DrawType::fill);

	//�A���t�@�u�����h�ɖ߂�
	sm.m_bs_Alpha->SetToDevice();

}

void KdEffectComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//��]�p�x
	ImGui::DragFloat(u8"��]�p�x", &m_zRotation, 0.1f, 0.0f, 360.0f);

	//���݂̃e�N�X�`���p�X
	ImGui::LabelText(m_textureFilePath.c_str(), u8"�\���e�N�X�`��\n");

	//�g�p����e�N�X�`���̑I��
	if (ImGui::Button(u8"�e�N�X�`���ύX"))
	{
		std::string selectFilePath = "";
		if (KdEditorData::GetInstance().OpenFileDialog(
			selectFilePath, "�摜�f�[�^�̑I��"))
		{
			//�摜�f�[�^�Ƃ��ēǂݍ��߂邩�ǂ���
			auto tex = KdResourceManager::GetInstance().LoadAsset<KdTexture>(selectFilePath);
			if (tex != nullptr)
			{
				m_texture = tex;
				m_textureFilePath = selectFilePath;
				//�e�N�X�`����ǂݍ���
				m_poly->SetTexture(tex);
			}
		}
		

	}
	
	//XY�g�k
	ImGui::DragFloat2(u8"�g�k", &m_scale.x, 0.05f, 0.0f, 10.0f);

	//�A�j���[�V�����Đ�����
	ImGui::DragFloat(u8"����", &m_lifetime, 0.01f, 0.1f, 100.0f);

	//�����̊g�k��
	ImGui::DragFloat2(u8"�����g�k��", &m_startScale.x, 0.01f, 0.0f, 100.0f);

	//�I�����̊g�k��
	ImGui::DragFloat2(u8"�I���g�k��", &m_endScale.x, 0.01f, 0.0f, 100.0f);

	//�������ɍĐ����邩
	ImGui::Checkbox(u8"�����Đ�", &m_playWithStart);

	//�Đ��{�^��
	if (ImGui::Button(u8"Play"))
	{
		StartAnim();

	}
	
}

void KdEffectComponent::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);
	
	KdJsonUtility::GetValue(jsonObj, "zRotate", &m_zRotation);
	KdJsonUtility::GetArray(jsonObj, "Scale", &m_scale.x, 2);
	KdJsonUtility::GetValue(jsonObj, "Lifetime", &m_lifetime);
	KdJsonUtility::GetArray(jsonObj, "StartScale", &m_startScale.x, 2);
	KdJsonUtility::GetArray(jsonObj, "EndScale", &m_endScale.x, 2);
	KdJsonUtility::GetValue(jsonObj, "PlayWith", &m_playWithStart);
	KdJsonUtility::GetValue(jsonObj, "TexturePath", &m_textureFilePath);
}

void KdEffectComponent::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["zRotate"] = m_zRotation;
	outJson["Scale"] = KdJsonUtility::CreateArray(&m_scale.x, 2);
	outJson["Lifetime"] = m_lifetime;
	outJson["StartScale"] = KdJsonUtility::CreateArray(&m_startScale.x, 2);
	outJson["EndScale"] = KdJsonUtility::CreateArray(&m_endScale.x, 2);
	outJson["PlayWith"] = m_playWithStart;
	outJson["TexturePath"] = m_textureFilePath;

}

void KdEffectComponent::StartAnim()
{
	m_isStarted = true;
	m_duration = 0.0f;
	m_ease.Start(KdEase::OutSine, m_lifetime);
}
