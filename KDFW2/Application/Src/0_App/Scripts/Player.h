#pragma once

//プレイヤーを制御するコンポーネント

class Player :public KdScriptComponent
{
public:
	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;

	//当たり判定関数
	void CollisionPhase(); 


	//Playerの情報を調整する
	virtual void Editor_ImGui()override;

	// このクラスの内容をJSONデータ化する(保存)
	virtual void Serialize(nlohmann::json& outJson) const override;
	

	// JSONデータから、クラスの内容を設定（読み込み）
	virtual void Deserialize(const nlohmann::json& jsonObj) override;
	

private:

	float m_speed=1.0f;
	float m_fall = 0.0f;

	//キャラクターの一回の移動量
	KdVector3 m_vMoveOnce = {};

	float m_jumpPow = 10.0f;

	float m_gravity = 0.915f;


	//自分で追加した変数
	float m_rayCorrection = 0.2f;
	float m_sphereCorrection = 0.5f;
	float m_sphereRadius = 0.3f;

	//アニメーション関係
	std::shared_ptr<KdAnimationComponent> m_animation = nullptr;

};