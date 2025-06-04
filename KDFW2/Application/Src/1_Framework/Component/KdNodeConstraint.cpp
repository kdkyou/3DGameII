#include "KdNodeConstraint.h"

#include"KdFramework.h"
#include"KdModelRendererComponent.h"

void KdNodeConstraint::Start()
{
	Connect();
}

bool KdNodeConstraint::Connect()
{
	// 親にModelComponentがあるか
	m_targetModel = GetGameObject()->GetComponentInParent< KdModelRendererComponent>();
	// 見つからなかった
	if (m_targetModel.expired() == true) { return false; }

	auto modelComp = m_targetModel.lock();

	// モデルコンポーネントの中から該当のノードを探す

	// 名前が入っているか
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



