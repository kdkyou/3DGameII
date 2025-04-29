#include "KdTransformComponent.h"

#include "KdGameObject.h"

#include "KdFramework.h"

#include "CoreSystem.h"

#include "Editor/KdEditorData.h"

SetClassAssembly(KdTransformComponent, "Component");

// ���[�J���s��擾
KdMatrix KdTransformComponent::GetLocalMatrix() const
{
	// ���W�A��]�A�g�傩��s����쐬
	return	DirectX::XMMatrixScalingFromVector(m_localScale) *
			DirectX::XMMatrixRotationQuaternion(m_localRotation) *
			DirectX::XMMatrixTranslationFromVector(m_localPosition);
}

// ���[���h�s��擾
const KdMatrix& KdTransformComponent::GetWorldMatrix() const
{
	// �s��X�V�K�v�t���O��ON�Ȃ�A�v�Z���s��
	if (m_matrixNeedUpdate)
	{
		// �e�ƍ���
		auto parent = GetGameObject()->GetParent();
		if (parent != nullptr)
		{
			m_calcedWorldMatrix = GetLocalMatrix() * parent->GetTransform()->GetWorldMatrix();
		}
		else
		{
			m_calcedWorldMatrix = GetLocalMatrix();
		}

		m_matrixNeedUpdate = false;
	}

	// �v�Z�ς݂̍s���Ԃ�
	return m_calcedWorldMatrix;
}

// ���[���h���W�Z�b�g
void KdTransformComponent::SetPosition(const KdVector3& pos)
{
	KdMatrix m = KdMatrix::CreateTranslation(pos);

	// ���[�J���ɕϊ�
	auto parent = GetGameObject()->GetParent();
	if (parent != nullptr)
	{
		m *= parent->GetTransform()->GetWorldMatrix().Invert();
	}

	SetLocalPosition(m.Translation());
}

// ���[���h��]�Z�b�g
void KdTransformComponent::SetRotation(const KdQuaternion& rotation)
{
	KdMatrix m = m.CreateFromQuaternion(rotation);

	// ���[�J���ɕϊ�
	auto parent = GetGameObject()->GetParent();
	if (parent != nullptr)
	{
		m *= parent->GetTransform()->GetWorldMatrix().Invert();
	}

	SetLocalRotation(KdQuaternion::CreateFromRotationMatrix(m));
}

// ���[�J���s����Z�b�g
void KdTransformComponent::SetLocalMatrix(const KdMatrix& mLocal)
{
	// �g��A��]�A���W�ɕ���
	DirectX::XMVECTOR s, r, t;
	DirectX::XMMatrixDecompose(&s, &r, &t, mLocal);

	m_localScale = s;
	m_localRotation = r;
	m_localPosition = t;

	// �ύX���}�[�N
	SetChanged();

}

// ���[���h�s����Z�b�g
void KdTransformComponent::SetWorldMatrix(const KdMatrix& mWorld)
{
	m_calcedWorldMatrix = mWorld;

	// ���[�J���ɕϊ�
	auto parent = GetGameObject()->GetParent();
	if (parent != nullptr)
	{
		KdMatrix mLocal = m_calcedWorldMatrix * parent->GetTransform()->GetWorldMatrix().Invert();
		SetLocalMatrix(mLocal);
	}
	else
	{
		SetLocalMatrix(m_calcedWorldMatrix);
	}
}

void KdTransformComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	KdMatrix mWorld = GetWorldMatrix();

	//-------------------------------
	// ���l����
	//-------------------------------
	KdVector3 pos;
	KdVector3 angles;
	KdVector3 scale;
	KdMatrix mLocal = GetLocalMatrix();
	
	ImGuizmo::DecomposeMatrixToComponents(&mLocal._11, &pos.x, &angles.x, &scale.x);

	// ���W
	if (ImGui::DragFloat3(u8"���W", &pos.x, 0.01f))
	{
		SetLocalPosition(pos);
	}

	// ��](�I�C���[�p)
	if (ImGui::DragFloat3(u8"��]", &angles.x, 1))
	{
		// ��]�p�x����N�H�[�^�j�I����
		KdMatrix m;
		ImGuizmo::RecomposeMatrixFromComponents(&KdVector3::Zero.x, &angles.x, &KdVector3::One.x, &m._11);
		SetLocalRotation(KdQuaternion::CreateFromRotationMatrix(m));
	}

	// �g��
	if (ImGui::DragFloat3(u8"�g��", &scale.x, 0.01f))
	{
		SetLocalScale(scale);
	}

	//-------------------------------
	// �}�j�s�����[�^
	//-------------------------------
	{
		KdMatrix mView = KdEditorData::GetInstance().m_editorCamera->GetCameraMatrix().Invert();
		KdMatrix mProj = KdEditorData::GetInstance().m_editorCamera->GetProjMatrix();



		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate(&mView._11, &mProj._11, KdEditorData::GetInstance().GizmoOperation, KdEditorData::GetInstance().GizmoMode, &mWorld._11);


		if (ImGuizmo::IsUsing())
		{
			SetWorldMatrix(mWorld);
		}
	}
}

void KdTransformComponent::SetChanged()
{
	m_matrixNeedUpdate = true;

	// �q���Z�b�g
	for (auto&& gameObj : GetGameObject()->GetChildren())
	{
		// ���łɕύX�t���O��ON�̎q�́A�X�L�b�v����(�����ȉ���ON�ɂȂ��Ă��邽��)
		if (gameObj->GetTransform()->m_matrixNeedUpdate)return;

		gameObj->GetTransform()->SetChanged();
	}
}
