#include"KdFramework.h"
#include"ObjectList.h"

#include"CheckList.h"

//�t���[�����[�N�ɃR���|�[�l���g�ł��邱�Ƃ�o�^
SetClassAssembly(ObjectList, "Component");

void ObjectList::Update()
{
	if (IsEnable() == false) { return; }

	//�S�Ă̗L��(Enable)�ȃI�u�W�F�N�g���擾����
	auto& enableObjects = KdFramework::GetInstance().GetScene()->GetCollectedObjects();

	m_grounds.clear();
	m_players.clear();
	m_enemys.clear();

	for (auto enable : enableObjects)
	{
		const auto&  checkList= enable->GetComponent<CheckList>();

		if (checkList == nullptr) { continue; }

//		if (checkList->IsDirty() == false) { continue; }

		//�n�`�I�u�W�F�N�g���ǂ���
		if (checkList->IsGround() == true) 
		{ 
			m_grounds.push_back(enable); 
		}

		//�G���ǂ���
		if (checkList->IsEnemy() == true)
		{
			m_enemys.push_back(enable);
		}

		//�v���C���[���ǂ���
		if (checkList->IsPlayer() == true) 
		{
			m_players.push_back(enable);
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

void ObjectList::Serialize(nlohmann::json& outJson) const
{
	KdScriptComponent::Serialize(outJson);
}

void ObjectList::Deserialize(const nlohmann::json& jsonObj)
{
	KdScriptComponent::Deserialize(jsonObj);
}
