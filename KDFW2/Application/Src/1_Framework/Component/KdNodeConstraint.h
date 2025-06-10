#pragma once
#include"KdComponent.h"

// �w�肵�����f�����̃m�[�h�ɑ΂��āA ���̃R���|�[�l���g���t����
// GameObject��Transform���֘A�t����

// ���̃R���|�[�l���g�͊֘A���郂�f���̎q�ǂ��ɔz�u����

class KdModelRendererComponent;

class KdNodeConstraint :public KdComponent
{
public :

	virtual void Start()override;
	virtual void Update()override;
	virtual void Editor_ImGui()override;

	// �w�肳�ꂽ�m�[�h�����邩���ׂ�
	bool Connect();	
	
	// �ۑ��ǂݍ���

	virtual void Serialize(nlohmann::json& outJson) const override;
	virtual void Deserialize(const nlohmann::json& jsonObj)override;

private:

	// �e��������m�[�h�̖��O
	std::string m_nodeName = "";

	// �m�[�h��GameObject�ɐ؂�ւ���
	bool m_reverse = false;

	// �e�v�f(���W�E��]�E�g�k)�̌Œ�@�\��ǉ�
	bool m_lockPos = false;
	KdVector3	m_localPos = {};
	
	bool m_lockRotate = false;
	KdVector3	m_localRotate = {};

	bool m_lockScale = false;
	KdVector3	m_localScale = { 1.0f,1.0f,1.0f };

	// �e�ɔz�u���ꂽ���f���R���|�[�l���g
	std::weak_ptr<KdModelRendererComponent> m_targetModel;
	// �������m�[�h�̃C���f�b�N�X
	int m_targetIndex = 1;

};