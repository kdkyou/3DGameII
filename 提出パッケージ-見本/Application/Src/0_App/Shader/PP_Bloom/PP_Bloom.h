#pragma once

#include "KdFramework.h"

class PP_Bloom : public KdPostProcessBase
{
public:
	virtual std::string GetName() const override { return "Bloom"; }

	virtual Timings GetTiming() override { return Timings::AfterTransparent; }
	virtual int32_t GetPriority() override { return 0; }

	void Initialize() override;

	virtual void Execute(KdScreenData& screenData) override;

	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSONデータから、クラスの内容を設定
	void Deserialize(const nlohmann::json& jsonObj) override
	{
		KdPostProcessBase::Deserialize(jsonObj);

		KdJsonUtility::GetValue(jsonObj, "m_brightThreshold", &m_brightThreshold);

		KdJsonUtility::GetValue(jsonObj, "m_sampleCount", &m_sampleCount);
		KdJsonUtility::GetValue(jsonObj, "m_dispersion", &m_dispersion);
	}

	// このクラスの内容をJSONデータ化する
	void Serialize(nlohmann::json& outJson) const override
	{
		KdPostProcessBase::Serialize(outJson);

		outJson["m_brightThreshold"] = m_brightThreshold;
		outJson["m_sampleCount"] = m_sampleCount;
		outJson["m_dispersion"] = m_dispersion;
	}


	// ImGui
	virtual void Editor_ImGui() override;

private:
	enum
	{
		kPass_DownSampling2x2 = 0,
		kPass_BrightFiltering = 1,
		kPass_Blur = 2,
	};

	static const int kBlurCount = 5;

	// 設定
	float m_brightThreshold = 0.9f;
	int m_sampleCount = 11;
	float m_dispersion = 3.0f;

	// デバッグ用
	bool m_DEBUG_ShowBlurOnly = false;

	// 
	std::shared_ptr<KdShader>			m_shader;

	std::shared_ptr<KdMaterial>			m_material;

	std::shared_ptr<KdRenderTexture>	m_highBrightness;
	std::shared_ptr<KdRenderTexture>	m_rt[kBlurCount][2];
};