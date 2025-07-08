#pragma once

/*
#include "Component/KdModelRendererComponent.h"

#include "KdGameObject.h"

//=========================================================
// 
// GameObjectを管理するクラス
// 
//=========================================================
class KdGameObjectCollection
{
public:

	// Root GameObjectを取得
	std::shared_ptr<KdGameObject> GetRootObject() { return m_rootObject; }

	// CollectGameObjects()で収集したGameObjectリストを取得
	const std::vector<KdGameObject*>& GetCollectedObjects() const { return m_collectedObjects; }

	// 初期設定
	void Initialize()
	{
		m_rootObject = std::make_shared<KdGameObject>();
		m_rootObject->Initialize("Root");
	}

	// 
	void ResetFrameData()
	{
		m_collectedObjects.clear();
	}

	// 全GameObjectから、有効なものを全て集める
	void CollectGameObjects()
	{
		m_rootObject->Collect(m_collectedObjects);
	}

	// Editor
	void Editor_ImGui();

private:

	// TopのGameObject
	std::shared_ptr<KdGameObject>			m_rootObject;

	// 収集されたGameObjectのリスト
	std::vector<KdGameObject*>				m_collectedObjects;
};
*/