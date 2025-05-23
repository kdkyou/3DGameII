#include"KdEffectComponent.h"

//KdPolygon�̒��ginclude
#include"0_App/Shader/Polygon/KdPolygon.h"
//�`�悷��̂ɕK�v
#include"../KdFramework.h"

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

	//�e�N�X�`�������̂��̂�ǂݍ���ł���
	m_poly->SetTexture("./Assets/Textures/Flare.png");

}

void KdEffectComponent::Update()
{

}

void KdEffectComponent::LateUpdate()
{

}


void KdEffectComponent::PreDraw()
{
	//���̃I�u�W�F�N�g���L����
	if (IsEnable() == false) { return; }

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
	KdVector3 scale = { m_scale.x,m_scale.y,1.0f };
	KdMatrix scaleMat = KdMatrix::CreateScale(scale);

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
	m_poly->Draw();

	//�A���t�@�u�����h�ɖ߂�
	sm.m_bs_Alpha->SetToDevice();

}

void KdEffectComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//��]�p�x
	ImGui::DragFloat(u8"��]�p�x", &m_zRotation, 0.1f, 0.0f, 360.0f);
	ImGui::DragFloat2(u8"�g�k", &m_scale.x, 0.1f, 0.1f, 10.0f);
	
}

void KdEffectComponent::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);
	
	KdJsonUtility::GetValue(jsonObj, "zRotate", &m_zRotation);
	KdJsonUtility::GetArray(jsonObj, "Scale", &m_scale.x, 2);

}

void KdEffectComponent::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["zRotate"] = m_zRotation;
	outJson["Scale"] = KdJsonUtility::CreateArray(&m_scale.x, 2);
}