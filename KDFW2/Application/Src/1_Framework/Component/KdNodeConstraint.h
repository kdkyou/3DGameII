#pragma once
#include"KdComponent.h"

// 指定したモデル内のノードに対して、 このコンポーネントが付いた
// GameObjectのTransformを関連付ける

// このコンポーネントは関連つけるモデルの子どもに配置する

class KdModelRendererComponent;

class KdNodeConstraint :public KdComponent
{
public :

	virtual void Start()override;
	virtual void Update()override;
	virtual void Editor_ImGui()override;

	// 指定されたノードがあるか調べる
	bool Connect();	
	
	// 保存読み込み

	virtual void Serialize(nlohmann::json& outJson) const override;
	virtual void Deserialize(const nlohmann::json& jsonObj)override;

private:

	// 影響させるノードの名前
	std::string m_nodeName = "";

	// ノード→GameObjectに切り替える
	bool m_reverse = false;

	// 各要素(座標・回転・拡縮)の固定機能を追加
	bool m_lockPos = false;
	KdVector3	m_localPos = {};
	
	bool m_lockRotate = false;
	KdVector3	m_localRotate = {};

	bool m_lockScale = false;
	KdVector3	m_localScale = { 1.0f,1.0f,1.0f };

	// 親に配置されたモデルコンポーネント
	std::weak_ptr<KdModelRendererComponent> m_targetModel;
	// 見つけたノードのインデックス
	int m_targetIndex = 1;

};