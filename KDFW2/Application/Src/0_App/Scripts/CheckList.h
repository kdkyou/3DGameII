#pragma once

class CheckList : public KdScriptComponent
{
public:

	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;
	virtual void Serialize(nlohmann::json& outJson) const override;			// (保存)
	virtual void Deserialize(const nlohmann::json& jsonObj) override;		// （読み込み）
	virtual void Editor_ImGui()override;													//チェックリスト用

	const bool IsGround() { return m_isGround; }
	const bool IsEnemy() { return m_isEnemy; }
	const bool IsPlayer() { return m_isPlayer; }

	//外部から呼び出し用
	const bool IsDirty();

private:

	bool m_isGround = false;		//地形
	bool  m_isEnemy = false;		//敵
	bool m_isPlayer = false;			//プレイヤー
	
	bool m_isDirty = false;			//フラグを切り替えたかどうか

};