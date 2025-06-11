#include"KdMoveRecorder.h"

#include"KdFramework.h"

SetClassAssembly(KdMoveRecorder, "Component");

void KdMoveRecorder::Start()
{
	// 最初のGameObjectの行列を覚えておく
	m_prev = GetGameObject()->GetTransform()->GetWorldMatrix();
}

void KdMoveRecorder::Update()
{
	const auto now = GetGameObject()->GetTransform();
	//前回の逆行列
	auto invPrev = m_prev.Invert();
	// 前回の逆行列 * 今の行列 = 2つの行列の差分
	m_difference = invPrev * now->GetWorldMatrix();


}

void KdMoveRecorder::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	ImGui::Text(u8"今回の移動量 x : %f, y : %f, z : %f", m_difference._41, m_difference._42, m_difference._43);
}
