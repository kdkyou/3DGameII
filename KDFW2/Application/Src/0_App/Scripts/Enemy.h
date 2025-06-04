#pragma once

class Collision;

class Enemy :public KdScriptComponent
{
public:

	virtual void Start() override;
	virtual void Update()override;
	virtual void LateUpdate()	override;


	// このクラスの内容をJSONデータ化する(保存)
	virtual void Serialize(nlohmann::json& outJson) const override;

	// JSONデータから、クラスの内容を設定（読み込み）
	virtual void Deserialize(const nlohmann::json& jsonObj) override;

	//エネミーの情報を調整する
	virtual void Editor_ImGui()override;

private:

	float m_speed = 2.0f;			//移動速度
	float m_ang = 3.0f;			//回転角度
	float m_difference = 3.0f;		//維持距離

	//Collisionコンポーネント
	std::shared_ptr<Collision>	m_spCollision = nullptr;

};