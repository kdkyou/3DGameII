#pragma once

#include "KdComponent.h"

#include "Math/KdMath.h"

class KdTransformComponent : public KdComponent
{
public:

	//=====================================
	// 
	// �f�[�^�擾�n
	// 
	//=====================================

	// ���[�J�����W�擾
	const KdVector3&		GetLocalPosition() const { return m_localPosition; }
	// ���[�J����]�擾
	const KdQuaternion&		GetLocalRotation() const { return m_localRotation; }
	// ���[�J���g��擾
	const KdVector3&		GetLocalScale() const { return m_localScale; }

	// ���[���h���W�擾
	KdVector3				GetPosition() const { return GetWorldMatrix().Translation(); }
	// ���[���h��]�擾
	KdQuaternion			GetRotation() const { return KdQuaternion::CreateFromRotationMatrix(GetWorldMatrix()); }

	// ���[�J���s��擾
	KdMatrix				GetLocalMatrix() const;
	// ���[���h�s��擾
	const KdMatrix&			GetWorldMatrix() const;

	//=====================================
	// 
	// �f�[�^�Z�b�g�n
	// 
	//=====================================
	// ���[�J�����W�Z�b�g
	void SetLocalPosition(const KdVector3& pos)
	{
		m_localPosition = pos;

		SetChanged();	// �ύX���}�[�N
	}
	// ���[�J����]�Z�b�g
	void SetLocalRotation(const KdQuaternion& rotation)
	{
		m_localRotation = rotation;

		SetChanged();	// �ύX���}�[�N
	}
	// ���[�J���g��Z�b�g
	void SetLocalScale(const KdVector3& scale)
	{
		m_localScale = scale;

		SetChanged();	// �ύX���}�[�N
	}

	// ���[���h���W�Z�b�g
	void SetPosition(const KdVector3& pos);
	// ���[���h��]�Z�b�g
	void SetRotation(const KdQuaternion& rotation);

	// ���[�J���s����Z�b�g
	void SetLocalMatrix(const KdMatrix& mLocal);
	// ���[���h�s����Z�b�g
	void SetWorldMatrix(const KdMatrix& mWorld);


	// �����ȉ��̎q�S�ĂɁA�ύX�t���O��ON�ɂ���
	void SetChanged();

	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		// �p�����̊֐������s
		KdComponent::Deserialize(jsonObj);

		KdJsonUtility::GetArray(jsonObj, "Position", &m_localPosition.x, 3);
		KdJsonUtility::GetArray(jsonObj, "Rotation", &m_localRotation.x, 4);
		KdJsonUtility::GetArray(jsonObj, "Scale", &m_localScale.x, 3);
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(nlohmann::json& outJson) const
	{
		// �p�����̊֐������s
		KdComponent::Serialize(outJson);

		outJson["Position"] = KdJsonUtility::CreateArray(&m_localPosition.x, 3);
		outJson["Rotation"] = KdJsonUtility::CreateArray(&m_localRotation.x, 4);
		outJson["Scale"] = KdJsonUtility::CreateArray(&m_localScale.x, 3);
	}

	//===============================
	// Editor
	//===============================
	virtual void Editor_ImGui() override;

private:

	// ���[�J�����W
	KdVector3			m_localPosition;
	// ���[�J����]
	KdQuaternion		m_localRotation;
	// ���[�J���g��
	KdVector3			m_localScale = { 1,1,1 };


	// �v�Z�ς݂̃��[���h�s��(�d���v�Z���o���邾��������p)
	mutable KdMatrix		m_calcedWorldMatrix;
	// �ύX����ON�ɂȂ�BON�̂�͍s��̍Čv�Z���K�v�ɂȂ�B
	mutable bool			m_matrixNeedUpdate = true;

};
