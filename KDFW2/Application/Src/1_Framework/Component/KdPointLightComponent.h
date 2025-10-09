#pragma once
#include"KdComponent.h"

//ポイントライト一つを管理するコンポーネント
class KdPointLightComponent :public KdComponent
{
public:

	virtual void Start()override;
	virtual void Update()override;
	virtual void Editor_ImGui()override;

	virtual void Deserialize(const nlohmann::json& jsonData)
	{
		KdComponent::Deserialize(jsonData);

		KdJsonUtility::GetArray(jsonData, "Color", &light.Color.x,3);
		KdJsonUtility::GetValue(jsonData, "Radius", &light.Radius);
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(nlohmann::json& outJson) const
	{
		KdComponent::Serialize(outJson);
		outJson["Color"] = KdJsonUtility::CreateArray(&light.Color.x, 3);
		outJson["Radius"] = light.Radius;
	}

private:
	// pointライトの情報
	KdShaderManager::CBLight::PointLight light;
	int pointNum = 0;

};