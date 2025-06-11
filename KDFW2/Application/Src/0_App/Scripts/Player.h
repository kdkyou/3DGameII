#pragma once

class Collision;

//プレイヤーを制御するコンポーネント
class Player :public KdScriptComponent
{
public:
	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;


	//Playerの情報を調整する
	virtual void Editor_ImGui()override;

	// このクラスの内容をJSONデータ化する(保存)
	virtual void Serialize(nlohmann::json& outJson) const override;
	

	// JSONデータから、クラスの内容を設定（読み込み）
	virtual void Deserialize(const nlohmann::json& jsonObj) override;
	

private:

	// 現在Playerが上に乗っているオブジェクト
	KdGameObject* m_ridingObject = nullptr;

	float m_speed=1.0f;
	
	//キャラクターの一回の移動量
	KdVector3 m_vMoveOnce = {};


	std::shared_ptr<Collision> m_spCollision = nullptr;

	//アニメーション関係
	std::shared_ptr<KdAnimationComponent> m_animation = nullptr;

};