#include"KdEffectComponent.h"

//KdPolygonの中身include
#include"0_App/Shader/Polygon/KdPolygon.h"

SetClassAssembly(KdEffectComponent, "Component");

void KdEffectComponent::Start()
{
	//ポリゴン作成
	m_poly = std::make_shared<KdPolygon>();
	m_poly->Initialize();

	//ポリゴンの形を作成	(1m*1m固定変更時はscaleをいじる)
	m_poly->AddVertex(KdVector3(-0.5f, -0.5f, 0), KdVector2(0.0f, 1.0f), 0xFFFFFFFF);		//左下
	m_poly->AddVertex(KdVector3(-0.5f, 0.5f, 0), KdVector2(0.0f, 0.0f), 0xFFFFFFFF);		//左上
	m_poly->AddVertex(KdVector3(0.5f, -0.5f, 0), KdVector2(1.0f, 1.0f), 0xFFFFFFFF);		//右下
	m_poly->AddVertex(KdVector3(0.5f, 0.5f, 0), KdVector2(1.0f, 0.0f), 0xFFFFFFFF);		//右上

	//テクスチャを仮のものを読み込んでおく
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