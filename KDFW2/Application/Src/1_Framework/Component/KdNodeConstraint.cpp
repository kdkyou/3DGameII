#include "KdNodeConstraint.h"

#include"KdFramework.h"
#include"KdModelRendererComponent.h"

void KdNodeConstraint::Start()
{
	Connect();
}

bool KdNodeConstraint::Connect()
{
	// �e��ModelComponent�����邩
	m_targetModel = GetGameObject()->GetComponentInParent< KdModelRendererComponent>();
	// ������Ȃ�����
	if (m_targetModel.expired() == true) { return false; }

	auto modelComp = m_targetModel.lock();

	// ���f���R���|�[�l���g�̒�����Y���̃m�[�h��T��

	// ���O�������Ă��邩
	if (m_nodeName != "") 
	{
		for(auto& index : modelComp->)



	}

	return true;
}

void KdNodeConstraint::Update()
{

}

void KdNodeConstraint::Editor_ImGui()
{

}



