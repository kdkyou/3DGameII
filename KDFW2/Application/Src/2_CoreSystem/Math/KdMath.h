#pragma once

#include <d3d11.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <SimpleMath.h>

#include "2_CoreSystem/nlohmann/KdJsonUtility.h"

namespace Math = DirectX::SimpleMath;

// �~����
static const float KdPI = 3.14159265358979323846f;
// ���W�A�� -> �x �ϊ��p
static const float KdRad2Deg = (180.f / KdPI);
// �x -> ���W�A�� �ϊ��p
static const float KdDeg2Rad = (KdPI / 180.f);

//===============================================================
// 2D�x�N�g��(SimpleMath�����̂܂܎g�p)
//===============================================================
using KdVector2 = DirectX::SimpleMath::Vector2;

//===============================================================
// 3D�x�N�g��(SimpleMath�����̂܂܎g�p)
//===============================================================
using KdVector3 = DirectX::SimpleMath::Vector3;

//===============================================================
// 4D�x�N�g��(SimpleMath�����̂܂܎g�p)
//===============================================================
using KdVector4 = DirectX::SimpleMath::Vector4;

//===============================================================
// �N�H�[�^�j�I��(SimpleMath�����̂܂܎g�p)
//===============================================================
using KdQuaternion = DirectX::SimpleMath::Quaternion;

//===============================================================
// 4x4�s��(SimpleMath�����̂܂܎g�p)
//===============================================================
using KdMatrix = DirectX::SimpleMath::Matrix;

//===============================================================
// �F(SimpleMath�����̂܂܎g�p)
//===============================================================
using KdColor = DirectX::SimpleMath::Color;

//===============================================================
//
// �g�����X�t�H�[��
// 
// �E���W�A��]�A�g��f�[�^�������A�s��ւ̑��ݕϊ����ł�����́B
// 
//===============================================================
class KdTransform
{
public:

	//=====================================
	// 
	// �f�[�^�擾�n
	// 
	//=====================================
	// (�擾)���[�J�����W
	const KdVector3&		GetLocalPosition() const { return m_localPosition; }
	// (�擾)���[�J����]
	const KdQuaternion&		GetLocalRotation() const { return m_localRotation; }
	// (�擾)���[�J���g��
	const KdVector3&		GetLocalScale() const { return m_localScale; }

	// (�擾)���[���h���W
	KdVector3				GetPosition() const { return GetWorldMatrix().Translation(); }
	// (�擾)���[���h��]
	KdQuaternion			GetRotation() const { return KdQuaternion::CreateFromRotationMatrix(GetWorldMatrix()); }

	// (�擾)���[�J���s��
	KdMatrix GetLocalMatrix() const
	{
		// ���W�A��]�A�g�傩��s����쐬
		return	DirectX::XMMatrixScalingFromVector(m_localScale) *
				DirectX::XMMatrixRotationQuaternion(m_localRotation) *
				DirectX::XMMatrixTranslationFromVector(m_localPosition);
	}

	// (�擾)���[���h�s��(�K�w�\���L��̍s��)
	const KdMatrix& GetWorldMatrix() const
	{
		// �s��X�V�K�v�t���O��ON�̎��̂݁A�v�Z���s��
		if (m_matrixNeedUpdate)
		{
			// �e�̍s��ƍ���
			if (m_parent != nullptr)
			{
				// �����̃��[�J���s�� * �e�̃��[���h�s��
				m_calcedWorldMatrix = GetLocalMatrix() * m_parent->GetWorldMatrix();
			}
			// �e����
			else
			{
				m_calcedWorldMatrix = GetLocalMatrix();
			}

			// �s��X�V�K�v�t���O��OFF
			m_matrixNeedUpdate = false;
		}

		// �v�Z�ς݂̍s���Ԃ�
		return m_calcedWorldMatrix;
	}

	//=====================================
	// 
	// �f�[�^�Z�b�g�n
	// 
	//=====================================
	// (���)���[�J�����W
	void SetLocalPosition(const KdVector3& pos)
	{
		m_localPosition = pos;

		// ���[���h�s����X�V����K�v������̂ŁA�t���O��ON�ɂ���
		SetChanged();
	}
	// (���)���[�J����]
	void SetLocalRotation(const KdQuaternion& rotation)
	{
		m_localRotation = rotation;

		// ���[���h�s����X�V����K�v������̂ŁA�t���O��ON�ɂ���
		SetChanged();
	}
	// (���)���[�J���g��
	void SetLocalScale(const KdVector3& scale)
	{
		m_localScale = scale;

		// ���[���h�s����X�V����K�v������̂ŁA�t���O��ON�ɂ���
		SetChanged();
	}

	// (���)���[���h���W
	void SetPosition(const KdVector3& pos)
	{
		// ���[�J���ɕϊ�
		KdMatrix m = KdMatrix::CreateTranslation(pos);
		if (m_parent != nullptr)
		{
			m *= m_parent->GetWorldMatrix().Invert();
		}

		SetLocalPosition(m.Translation());
	}
	// (���)���[���h��]
	void SetRotation(const KdQuaternion& rotation)
	{
		// ���[�J���ɕϊ�
		KdMatrix m = m.CreateFromQuaternion(rotation);
		if (m_parent != nullptr)
		{
			m *= m_parent->GetWorldMatrix().Invert();
		}

		SetLocalRotation(KdQuaternion::CreateFromRotationMatrix(m));
	}


	// (���)���[�J���s��
	void SetLocalMatrix(const KdMatrix& mLocal)
	{
		// �g��A��]�A���W�ɕ���
		DirectX::XMVECTOR s, r, t;
		DirectX::XMMatrixDecompose(&s, &r, &t, mLocal);

		m_localScale = s;
		m_localRotation = r;
		m_localPosition = t;

		// ���[���h�s����X�V����K�v������̂ŁA�t���O��ON�ɂ���
		SetChanged();
	}

	// (���)���[���h�s��
	void SetWorldMatrix(const KdMatrix& mWorld)
	{
		m_calcedWorldMatrix = mWorld;

		if (m_parent != nullptr)
		{
			// ���[�J���s��ɕϊ�
			KdMatrix mLocal = m_calcedWorldMatrix * m_parent->GetWorldMatrix().Invert();

			// ���[�J���s��Z�b�g
			SetLocalMatrix(mLocal);
		}
		else
		{
			// ���[�J���s��Z�b�g
			SetLocalMatrix(m_calcedWorldMatrix);
		}
	}

	//=====================================
	// 
	// �V���A���C�Y/�f�V���A���C�Y
	// 
	//=====================================

	// JSON��
	nlohmann::json ToJson() const
	{
		nlohmann::json js = nlohmann::json::object();
		js["Position"] = KdJsonUtility::CreateArray(&m_localPosition.x, 3);
		js["Rotation"] = KdJsonUtility::CreateArray(&m_localRotation.x, 4);
		js["Scale"] = KdJsonUtility::CreateArray(&m_localScale.x, 3);
		return js;
	}
	// JSON����f�[�^�Z�b�g
	void FromJson(const nlohmann::json &jsonObj)
	{
		KdJsonUtility::GetArray(jsonObj, "Position", &m_localPosition.x, 3);
		KdJsonUtility::GetArray(jsonObj, "Rotation", &m_localRotation.x, 4);
		KdJsonUtility::GetArray(jsonObj, "Scale",	&m_localScale.x, 3);
	}

private:

	// �����ȉ��̎q�S�ĂɁA���[���h�s��X�V�t���O��ON�ɂ���
	void SetChanged()
	{
		// ���[���h�s��̕ύX���K�v�t���O��ON�ɂ���
		m_matrixNeedUpdate = true;

		// �q���Z�b�g����(�ċA)
		for (auto&& child : m_children)
		{
			// ���łɕύX�t���O��ON�̎q�́A�X�L�b�v����(�����ȉ���ON�ɂȂ��Ă��邽��)
			if (child->m_matrixNeedUpdate)return;

			child->SetChanged();
		}
	}

private:

	// ���[�J�����W
	KdVector3			m_localPosition;
	// ���[�J����]
	KdQuaternion		m_localRotation;
	// ���[�J���g��
	KdVector3			m_localScale = { 1,1,1 };

	//---------------
	// �K�w�\����
	//---------------
	// �v�Z�ς݃��[���h�s��
	mutable KdMatrix	m_calcedWorldMatrix;

	// ���[���h�s��ύX�K�v�t���O
	// ���ꂪON�̎��́A���[���h�s����Čv�Z����K�v������
	mutable bool		m_matrixNeedUpdate = true;

public:
	// �K�w�\���ɂ���ꍇ�A�e�ւ̎Q��
	KdTransform*				m_parent = nullptr;
	// �K�w�\���ɂ���ꍇ�A�q�ւ̎Q�Ɣz��
	std::vector<KdTransform*>	m_children;
};
