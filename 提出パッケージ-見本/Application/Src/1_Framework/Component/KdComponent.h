#pragma once

class KdGameObject;

//=====================================================
//
// コンポーネント基本クラス
//
//=====================================================
class KdComponent : public KdObject
{
public:

	// 持ち主設定
	void SetGameObject(std::shared_ptr<KdGameObject> obj) { m_pGameObject = obj; }
	// 持ち主取得
	std::shared_ptr<KdGameObject> GetGameObject() const { return m_pGameObject.lock(); }
	// 持ち主が存在するか
//	bool ExistOwner() const { return m_pGameObject.expired() == false; }

	// 有効フラグ設定
	void SetEnable(bool enable) { m_enable = enable; }
	// 有効フラグ取得
	bool IsEnable() const { return m_enable; }

	// 初回に１度だけ実行される関数
	virtual void Start() {}

	// 更新処理
	virtual void Update() {}
	// 更新処理
	virtual void LateUpdate() {}

	// 描画準備処理
	virtual void PreDraw() {}

	// 描画処理
	virtual void Draw(int phaseID) {}


	
	// 初回のみStart関数を呼び出す
	void CallStartOnce()
	{
		// 初回フラグがOnの時のみ
		if (m_isNew)
		{
			Start();		// Start実行
			m_isNew = false;
		}
	}
	

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const nlohmann::json& jsonData)
	{
		m_enable = jsonData["Enable"].get<bool>();
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(nlohmann::json& outJson) const
	{
		outJson["ClassName"] = GetClassName();
		outJson["Enable"] = m_enable;
	}

	//===============================
	// Editor
	//===============================
	virtual void Editor_ImGui();

	//===============================
	// その他
	//===============================

protected:

	// 有効/無効フラグ
	bool		m_enable = true;

	// 作成したて？のフラグ
	bool		m_isNew = true;

	// 持ち主へのアドレス
	std::weak_ptr<KdGameObject> m_pGameObject;
};

// 描画が必要なコンポーネント
class KdRendererComponent : public KdComponent
{
public:
	virtual void Draw(bool opaque, KdShader::PassTags passTag) {}
};