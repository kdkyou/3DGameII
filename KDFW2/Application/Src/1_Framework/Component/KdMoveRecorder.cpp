#include"KdMoveRecorder.h"

#include"KdFramework.h"

SetClassAssembly(KdMoveRecorder, "Component");

void KdMoveRecorder::Start()
{
	// �ŏ���GameObject�̍s����o���Ă���
	m_prev = GetGameObject()->GetTransform()->GetWorldMatrix();
}

void KdMoveRecorder::Update()
{
	const auto now = GetGameObject()->GetTransform();
	//�O��̋t�s��
	auto invPrev = m_prev.Invert();
	// �O��̋t�s�� * ���̍s�� = 2�̍s��̍���
	m_difference = invPrev * now->GetWorldMatrix();


}

void KdMoveRecorder::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	ImGui::Text(u8"����̈ړ��� x : %f, y : %f, z : %f", m_difference._41, m_difference._42, m_difference._43);
}
