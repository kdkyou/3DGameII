#include"KdFramework.h"
#include"CheckList.h"

//フレームワークにコンポーネントであることを登録
SetClassAssembly(CheckList, "Component");

void CheckList::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["Player"] = m_isPlayer;
	outJson["Ground"] = m_isGround;
	outJson["Enemy"] = m_isEnemy;

}

void CheckList::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "Player", &m_isPlayer);
	KdJsonUtility::GetValue(jsonObj, "Ground", &m_isGround);
	KdJsonUtility::GetValue(jsonObj, "Enemy", &m_isEnemy);
}

void CheckList::Editor_ImGui()
{
	if (ImGui::Checkbox(u8"プレイヤー", &m_isPlayer)) {
		m_isDirty = true;
	}
	if(	ImGui::Checkbox(u8"地形", &m_isGround)) {
		m_isDirty = true;
	}
	if(ImGui::Checkbox(u8"敵", &m_isEnemy)) {
		m_isDirty = true;
	}

}

const bool CheckList::IsDirty()
{
	if (m_isDirty == true) {
		m_isDirty = false;
		return true;
	}
	else {
		return false;
	}

}
