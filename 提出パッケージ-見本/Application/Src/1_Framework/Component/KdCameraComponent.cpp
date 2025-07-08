#include "KdCameraComponent.h"

#include "KdFramework.h"

SetClassAssembly(KdCameraComponent, "Component");


inline KdCameraComponent::KdCameraComponent()
{
	m_screenData = std::make_shared<KdScreenData>();
	m_screenData->Initialize();
}

KdMatrix KdCameraComponent::GetCameraMatrix() const
{
	return GetGameObject()->GetTransform()->GetWorldMatrix();
}

KdMatrix KdCameraComponent::GetProjMatrix() const
{
	float aspect = m_screenData->GetScreenAspect();

	return DirectX::XMMatrixPerspectiveFovLH(m_proj_FOV * KdDeg2Rad, aspect, m_proj_NearAndFar.x, m_proj_NearAndFar.y);
}

void KdCameraComponent::Update()
{
	KdFramework::GetInstance().m_renderingData.m_cameras.push_back(this);
}

void KdCameraComponent::WriteCameraCBuffer()
{
	// カメラの定数バッファにデータをセット
	KdShaderManager::GetInstance().m_cbPerCamera->EditCB().SetDatas(GetCameraMatrix(), GetProjMatrix());

	KdShaderManager::GetInstance().m_cbPerCamera->WriteWorkData();
}

void KdCameraComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	if (ImGui::CollapsingHeader(u8"視野設定", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::DragFloat(u8"視野角", &m_proj_FOV, 1, 1, 179);
		ImGui::DragFloat2(u8"範囲", &m_proj_NearAndFar.x, 0.1f, 0.01f, 5000);
		if (m_proj_NearAndFar.y < m_proj_NearAndFar.x)
		{
			m_proj_NearAndFar.y = m_proj_NearAndFar.x + 0.01f;
		}
	}

	
}
