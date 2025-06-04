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

private:

	// 影響させるノードの名前
	std::string m_nodeName = "";
	// 親に配置されたモデルコンポーネント
	std::weak_ptr<KdModelRendererComponent> m_targetModel;
	// 見つけたノードのインデックス
	int m_targetIndex = 1;

};