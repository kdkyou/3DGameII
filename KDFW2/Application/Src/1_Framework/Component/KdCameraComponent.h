#pragma once

#include "KdComponent.h"

#include "Math/KdMath.h"

class KdScreenData;

//=========================================================
// 
//=========================================================
class KdCameraComponent : public KdComponent
{
public:

	// 
	std::shared_ptr<KdScreenData> GetScreenData() { return m_screenData; }

	KdCameraComponent();

	virtual void Update() override;

	virtual void Draw() {}

	void WriteCameraCBuffer();

	KdMatrix GetCameraMatrix() const;

	KdMatrix GetProjMatrix() const;

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		// 継承元の関数も実行
		KdComponent::Deserialize(jsonObj);

		KdJsonUtility::GetValue(jsonObj, "FOV", &m_proj_FOV);
		KdJsonUtility::GetArray(jsonObj, "FearAndFar", &m_proj_NearAndFar.x, 2);

	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(nlohmann::json& outJson) const
	{
		// 継承元の関数も実行
		KdComponent::Serialize(outJson);

		outJson["FOV"] = m_proj_FOV;
		outJson["FearAndFar"] = KdJsonUtility::CreateArray(&m_proj_NearAndFar.x, 2);
	}

	virtual void Editor_ImGui() override;

private:

	// 射影行列用データ
	float			m_proj_FOV = 60;						// 視野角
	KdVector2		m_proj_NearAndFar = { 0.01f, 5000 };	// 最近接距離と最遠方距離


	// 描画先画面
	std::shared_ptr<KdScreenData> m_screenData;
};
