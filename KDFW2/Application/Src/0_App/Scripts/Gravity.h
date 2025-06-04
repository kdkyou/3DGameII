#pragma once

class Player;

class Gravity :public KdScriptComponent
{
public:
	virtual void Start()		override;
	virtual void Update()		override;
	virtual void LateUpdate()	override;
	

	//Gravityの情報を調整する
	virtual void Editor_ImGui()override;

	// このクラスの内容をJSONデータ化する(保存)
	virtual void Serialize(nlohmann::json& outJson) const override;


	// JSONデータから、クラスの内容を設定（読み込み）
	virtual void Deserialize(const nlohmann::json& jsonObj) override;

	const KdVector3& GetMoveOnes() const { return m_vMoveOnes; }

	void Ground() { m_isGround = true; }

private:

	//着地判定フラグ
	bool m_isGround = false;

	float m_gravity = 0.915f;	//重力
	float m_fall = 0.0f;		//加算された移動量
	KdVector3 m_vMoveOnes = { 0,0,0 };

	float m_jumpPow = 2.0f;		//ジャンプ力

	float m_intervalVal = 1.0f;	//自動のインターバルの値
	float m_interval = 0.0f;	//自動のインターバル

	//キー入力するか自動か
	int m_isSelect = 0;

	enum Select
	{
		None,
		Key,
		Auto,
	};
};