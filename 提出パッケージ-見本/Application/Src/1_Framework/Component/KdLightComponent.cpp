#include "KdLightComponent.h"

#include "KdFramework.h"

SetClassAssembly(KdLightComponent, "Component");

void KdLightComponent::Update()
{
	// ƒ‰ƒCƒg‚ð“o˜^
	KdFramework::GetInstance().m_renderingData.m_lights.push_back(this);
}

void KdLightComponent::WriteLightDataToCBuffer()
{
	if (m_lightType == LightTypes::Directional)
	{
		auto& m = GetGameObject()->GetTransform()->GetWorldMatrix();

		auto& cb = KdShaderManager::GetInstance().m_cbLight->EditCB();

		cb.DL_Dir = m.Backward();
		cb.DL_Dir.Normalize();

		cb.DL_Color = (KdVector3&)m_color;
	}
}

void KdLightComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	ImGui::ColorEdit3("Color", &m_color.x, ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float);
}
