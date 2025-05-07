#include"KdFramework.h"
#include"ObjectList.h"

#include"CheckList.h"

//フレームワークにコンポーネントであることを登録
SetClassAssembly(ObjectList, "Component");

void ObjectList::Update()
{
	if (IsEnable() == false) { return; }

	//全ての有効(Enable)なオブジェクトを取得する
	auto& enableObjects = KdFramework::GetInstance().GetScene()->GetCollectedObjects();

	for (auto enable : enableObjects)
	{
		const auto&  checkList= enable->GetComponent<CheckList>();

		if (checkList == nullptr) { continue; }

		if (checkList->IsDirty() == false) { continue; }

		//地形オブジェクトかどうか
		if (checkList->IsGround() == true) 
		{ 
		//	m_grounds.push_back(enable); 
			int a = 0;
		}

		//敵かどうか
		if (checkList->IsEnemy() == true)
		{
		//	m_enemys.push_back(enable);
			int a = 0;
		}

		//プレイヤーかどうか
		if (checkList->IsPlayer() == true) 
		{
		//	m_players.push_back(enable);
			int a = 0;
		}
	}

}

void ObjectList::Editor_ImGui()
{
	/*for (auto&& objs : m_grounds)
	{
		ImGui::Text(objs->GetName().c_str());
	}*/

}
