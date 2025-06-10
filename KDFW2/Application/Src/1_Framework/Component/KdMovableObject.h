#pragma once

// 移動可能オブジェクトの移動を管理する

// 仕様
// GameObject←KdMovableObject
//		L GameObject(実際に動くキャラクター)
//		L GameObject(開始地点)
//		L GameObject(終了地点)

#include"KdComponent.h"

class KdMovableObject :public KdComponent
{
public:

	virtual  void Start()override;
	virtual void Update()override;
	virtual void Editor_ImGui()override;

	virtual void Serialize(nlohmann::json& outJson) const override;			// (保存)
	virtual void Deserialize(const nlohmann::json& jsonObj) override;		// （読み込み）

	void Set();	// 移動ポイントを設定
private:

	// 実際に動作させるGameObject
	std::weak_ptr<KdGameObject>	m_moveObject;

	// 開始地点・終了地点
	std::weak_ptr<KdGameObject>	m_start;
	std::weak_ptr<KdGameObject>	m_end;

	// 一回の動作に必要な秒数
	float m_oneMoveTime = 5.0f;
	// 往復対応
	bool m_roundTrip = true;
	// ポイントでの待ち時間
	float m_waitTime = 2.0f;
	// 複数ポイントの対応
	// 先頭オブジェクト＝動くオブジェクト
	// 以降のオブジェクトがスタート地点からの経由地
	std::vector<std::weak_ptr<KdGameObject>>m_wayPoints;


	//現在の経過時間
	float m_duration = 0.0f;
	float m_waitduration = 0.0f;
	int   m_wayNum = 0;

};