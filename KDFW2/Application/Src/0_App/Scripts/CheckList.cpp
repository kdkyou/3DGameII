#include"KdFramework.h"
#include"CheckList.h"

//�t���[�����[�N�ɃR���|�[�l���g�ł��邱�Ƃ�o�^
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
	if (ImGui::Checkbox(u8"�v���C���[", &m_isPlayer)) {
		m_isDirty = true;
	}
	if(	ImGui::Checkbox(u8"�n�`", &m_isGround)) {
		m_isDirty = true;
	}
	if(ImGui::Checkbox(u8"�G", &m_isEnemy)) {
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
