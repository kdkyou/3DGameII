#pragma once

//当たり判定を制御するコンポーネント

class Collision :public KdScriptComponent
{
public:

	struct RayColInfo
	{
		std::string Name = "";
		bool isEnable = true;
		KdVector3 startPos = {0,0,0};
		KdVector3 correction = {0,0,0};
		KdVector3 direction = { 0,0,0 };
	};

	struct SphereColInfo
	{
		std::string Name = "";
		bool isEnable = true;
		bool isResult = true;
		KdVector3 center = { 0,0.5f,0 };
		float radius = 0.5f;
		KdVector3 vPushBack = {0,0,0};
	};


	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;

	// このクラスの内容をJSONデータ化する(保存)
	virtual void Serialize(nlohmann::json& outJson) const override;

	// JSONデータから、クラスの内容を設定（読み込み）
	virtual void Deserialize(const nlohmann::json& jsonObj) override;

	//当たり判定の情報を調整する
	virtual void Editor_ImGui()override;

private:

	void AddRay();
	void AddSphere();

	std::vector<RayColInfo> m_rayDatas;
	std::vector<SphereColInfo>m_sphereDatas;
	
}; 