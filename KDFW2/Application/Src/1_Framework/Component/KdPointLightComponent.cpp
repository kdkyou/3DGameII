#include "KdPointLightComponent.h"
#include"KdFramework.h"

SetClassAssembly(KdPointLightComponent, "Component");

void KdPointLightComponent::Start()
{
	pointNum = KdShaderManager::GetInstance().UseNumber();
	
}

void KdPointLightComponent::Update()
{
	if (pointNum < 0)
	{
		m_enable = false;
	}

	if (m_pGameObject.expired() == true)
	{
		KdShaderManager::GetInstance().ResetLight(pointNum);
	}

	// ポイントライトの場所=このコンポーネントが付いたオブジェクトの場所
	light.enable = m_enable;
	light.Pos = GetGameObject()->GetTransform()->GetPosition();
	KdShaderManager::GetInstance().SetPointLight(light,pointNum);
}

void KdPointLightComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	ImGui::ColorEdit3(u8"Color", &light.Color.x,
		ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
	ImGui::DragFloat(u8"Radius", &light.Radius, 0.1f, 0.0f, 100.0f);

}
