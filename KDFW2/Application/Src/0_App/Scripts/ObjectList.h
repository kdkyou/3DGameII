#pragma once

class ObjectList :public KdScriptComponent
{
public :

	void Update()override;

	virtual void Editor_ImGui()override;													//リスト確認用
	virtual void Serialize(nlohmann::json& outJson) const override;			// (保存)
	virtual void Deserialize(const nlohmann::json& jsonObj) override;		// （読み込み）

	//外部にリスト提供
	const std::list<KdGameObject*>& GetPlayers() { return m_players; }
	const std::list<KdGameObject*>& GetGrounds() { return m_grounds; }
	const std::list<KdGameObject*>& GetEnemys() { return m_enemys; }
	
private:

	std::list<KdGameObject*> m_players;
	std::list<KdGameObject*> m_grounds;
	std::list<KdGameObject*> m_enemys;

};