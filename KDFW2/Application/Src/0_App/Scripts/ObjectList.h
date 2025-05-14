#pragma once

class ObjectList :public KdScriptComponent
{
public :

	void Update()override;

	virtual void Editor_ImGui()override;													//���X�g�m�F�p
	virtual void Serialize(nlohmann::json& outJson) const override;			// (�ۑ�)
	virtual void Deserialize(const nlohmann::json& jsonObj) override;		// �i�ǂݍ��݁j

	//�O���Ƀ��X�g��
	const std::list<KdGameObject*>& GetPlayers() { return m_players; }
	const std::list<KdGameObject*>& GetGrounds() { return m_grounds; }
	const std::list<KdGameObject*>& GetEnemys() { return m_enemys; }
	
private:

	std::list<KdGameObject*> m_players;
	std::list<KdGameObject*> m_grounds;
	std::list<KdGameObject*> m_enemys;

};