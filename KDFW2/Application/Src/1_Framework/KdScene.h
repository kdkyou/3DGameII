#pragma once

#include "Component/KdModelRendererComponent.h"

#include "KdGameObject.h"

//=========================================================
// 
// シーン
// Hierarchyを管理しているクラス
// 
//=========================================================
class KdScene : public KdObject
{
public:
	//プレハブファイルからGameObjectを動的生成
	//戻り値：新しく作成したGameObject
	//prefabFilePat：参考にするプレハブファイル
	//parent：新しく生成されたGameObjectの親
	//		  指定がなかったらRoot直下に作る
	std::shared_ptr<KdGameObject>	Instantiate(
		const std::string& prefabFilePath,
		std::shared_ptr<KdGameObject> parent = nullptr
	);

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

	// 
	void Update()
	{
		// 収集したGameObjectのUpdateを実行する
		for (auto&& gameObj : m_collectedObjects)
		{
			//新たに登録されたオブジェクトのStartを呼び出していく
			for (auto&& comp : gameObj->GetComponentList())
			{
				comp->CallStartOnce();
			}

			for (auto&& comp : gameObj->GetComponentList())
			{
				comp->Update();
			}
		}
	}

	void Release()
	{
		m_rootObject = nullptr;

		m_collectedObjects.clear();
	}

	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSONデータから、クラスの内容を設定
	void Deserialize(const nlohmann::json& jsonObj);
	// このクラスの内容をJSONデータ化する
	void Serialize(nlohmann::json& outJson) const;

	// ファイルから読み込み
	bool LoadFromFile(const std::string& filepath);

	// ファイルへ保存
	bool SaveToFile(const std::string& filepath);

	//===============================
	// Editor
	//===============================
	void Editor_ImGui();

private:

	// TopのGameObject
	std::shared_ptr<KdGameObject>			m_rootObject;

	// 収集されたGameObjectのリスト
	std::vector<KdGameObject*>				m_collectedObjects;

};
