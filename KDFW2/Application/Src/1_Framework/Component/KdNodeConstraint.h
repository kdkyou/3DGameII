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

private:

	// �e��������m�[�h�̖��O
	std::string m_nodeName = "";
	// �e�ɔz�u���ꂽ���f���R���|�[�l���g
	std::weak_ptr<KdModelRendererComponent> m_targetModel;
	// �������m�[�h�̃C���f�b�N�X
	int m_targetIndex = 1;

};