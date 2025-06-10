#pragma once

// �ړ��\�I�u�W�F�N�g�̈ړ����Ǘ�����

// �d�l
// GameObject��KdMovableObject
//		L GameObject(���ۂɓ����L�����N�^�[)
//		L GameObject(�J�n�n�_)
//		L GameObject(�I���n�_)

#include"KdComponent.h"

class KdMovableObject :public KdComponent
{
public:

	virtual  void Start()override;
	virtual void Update()override;
	virtual void Editor_ImGui()override;

	virtual void Serialize(nlohmann::json& outJson) const override;			// (�ۑ�)
	virtual void Deserialize(const nlohmann::json& jsonObj) override;		// �i�ǂݍ��݁j

	void Set();	// �ړ��|�C���g��ݒ�
private:

	// ���ۂɓ��삳����GameObject
	std::weak_ptr<KdGameObject>	m_moveObject;

	// �J�n�n�_�E�I���n�_
	std::weak_ptr<KdGameObject>	m_start;
	std::weak_ptr<KdGameObject>	m_end;

	// ���̓���ɕK�v�ȕb��
	float m_oneMoveTime = 5.0f;
	// �����Ή�
	bool m_roundTrip = true;
	// �|�C���g�ł̑҂�����
	float m_waitTime = 2.0f;
	// �����|�C���g�̑Ή�
	// �擪�I�u�W�F�N�g�������I�u�W�F�N�g
	// �ȍ~�̃I�u�W�F�N�g���X�^�[�g�n�_����̌o�R�n
	std::vector<std::weak_ptr<KdGameObject>>m_wayPoints;


	//���݂̌o�ߎ���
	float m_duration = 0.0f;
	float m_waitduration = 0.0f;
	int   m_wayNum = 0;

};