#pragma once


#include "Component/KdTransformComponent.h"

//=============================================================
// 
// GameObject
// 
//=============================================================
class KdGameObject final : public KdObject
{
public:

	KdGameObject() {}

	// 名前設定
	void SetName(const std::string& name) { m_name = name; }
	// 名前取得
	const std::string& GetName() const { return m_name; }

	// 有効フラグ設定
	void SetEnable(bool enable) { m_enable = enable; }
	// 有効フラグ取得
	bool IsEnable() const { return m_enable; }

	// Tag設定
	void SetTag(const std::string& tag){m_tag = tag;}
	const std::string& GetTag() const {return m_tag;}

	// Transform取得
	std::shared_ptr<KdTransformComponent> GetTransform() const { return std::static_pointer_cast<KdTransformComponent>(m_components[0]); }

	// 自分や自分の子たち対象に、有効なGameObjectをgameObjectListへ登録する
	void Collect(std::vector<KdGameObject*>& gameObjectList);

	// 初期設定
	void Initialize(const std::string& name);

	//===============================
	//
	// コンポーネント
	//
	//===============================

	// コンポーネントリスト取得
	const std::vector<std::shared_ptr<KdComponent>>& GetComponentList() const { return m_components; }

	// コンポーネント追加
	template<class T>
	std::shared_ptr<T> AddComponent()
	{
		auto comp = std::make_shared<T>();
		AddComponent(comp);
		return comp;
	}
	// コンポーネント追加(クラス名を指定)
	std::shared_ptr<KdComponent>AddComponent(const std::string& className);

	// コンポーネント追加
	void AddComponent(const std::shared_ptr<KdComponent>& comp);

	// 指定型のコンポーネント取得
	template<class T>
	std::shared_ptr<T> GetComponent()
	{
		// 検索
		for (auto&& comp : m_components)
		{
			// ダウンキャストで型判定
			auto p = std::dynamic_pointer_cast<T>(comp);
			if (p)return p;
		}
		return nullptr;
	}

	// 指定型のコンポーネント取得(子を含む)
	template<class T>
	std::shared_ptr<T> GetComponentInChildren()
	{
		auto comp = GetComponent<T>();
		if (comp != nullptr) return comp;

		// 子を検索する
		for (auto&& child : m_children)
		{
			comp = child->GetComponentInChildren<T>();
			if (comp != nullptr) return comp;
		}

		return nullptr;
	}

	// 指定型のコンポーネントを全て取得
	// ・result		… 見つかったコンポーネントが全てここに入る 
	// ・withChild	… 子GameObjectも検索する？
	template<class T>
	void GetComponents(std::vector<std::shared_ptr<T>>& result)
	{
		// 検索
		for (auto&& comp : m_components)
		{
			// ダウンキャストで型判定
			auto p = std::dynamic_pointer_cast<T>(comp);
			if (p != nullptr) 
				result.push_back(p);
		}
	}

	// 指定型のコンポーネントを全て取得
	// ・result		… 見つかったコンポーネントが全てここに入る 
	// ・withChild	… 子GameObjectも検索する？
	template<class T>
	void GetComponentsInChildren(std::vector<std::shared_ptr<T>>& result)
	{
		// 検索
		for (auto&& comp : m_components)
		{
			// ダウンキャストで型判定
			auto p = std::dynamic_pointer_cast<T>(comp);
			if (p != nullptr)
				result.push_back(p);
		}
		// 子も検索する
		for (auto&& child : m_children)
		{
			child->GetComponentsInChildren(result);
		}
	}

	// 指定型のコンポーネント取得(親方向を含む)
	template<class T>
	std::shared_ptr<T> GetComponentInParent()
	{
		auto comp = GetComponent<T>();
		if (comp != nullptr) return comp;

		auto parent = m_parent.lock();
		if (parent == nullptr) return nullptr;

		return parent->GetComponentInParent<T>();
	}

	//===============================
	// 
	// 階層構造
	// 
	//===============================

	// 親取得
	std::shared_ptr<KdGameObject> GetParent() const { return m_parent.lock(); }
	void SetParent(const std::shared_ptr<KdGameObject>& newParent, bool worldPositionStays);

	// 子リスト取得
	const std::list<std::shared_ptr<KdGameObject>>& GetChildren() { return m_children; }

	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSONデータから、クラスの内容を設定
	void Deserialize(const nlohmann::json& jsonObj);
	// このクラスの内容をJSONデータ化する
	void Serialize(nlohmann::json& outJson) const;


	//===============================
	// Editor
	//===============================
	void Editor_ImGui();

private:

	// 固有ID
	KdGuid									m_guid;

	// 名前
	std::string								m_name = "GameObject";

	// 有効フラグ
	bool									m_enable = true;

	// Tag
	std::string								m_tag = "";

	// コンポーネントリスト
	std::vector<std::shared_ptr<KdComponent>>		m_components;

	// 子リスト
	std::list<std::shared_ptr<KdGameObject>>		m_children;
	// 親のポインタ
	std::weak_ptr<KdGameObject>						m_parent;
};
