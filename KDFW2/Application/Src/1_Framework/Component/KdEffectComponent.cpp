#include"KdEffectComponent.h"

//KdPolygon�̒��ginclude
#include"0_App/Shader/Polygon/KdPolygon.h"

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

}

void KdEffectComponent::Draw(bool opaque, KdShader::PassTags passTag)
{
	m_poly->Draw();
}



void KdEffectComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();
}