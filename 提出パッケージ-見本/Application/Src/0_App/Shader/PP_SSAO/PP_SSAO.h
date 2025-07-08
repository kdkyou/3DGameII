#pragma once

#include "KdFramework.h"

class PP_SSAO : public KdPostProcessBase
{
public:

	virtual std::string GetName() const override { return "SSAO"; }

	virtual Timings GetTiming() override { return Timings::AfterOpaque; }
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

		KdJsonUtility::GetValue(jsonObj, "m_sampleCount", &m_sampleCount);
		KdJsonUtility::GetValue(jsonObj, "m_radius", &m_radius);
		KdJsonUtility::GetValue(jsonObj, "m_sigma", &m_sigma);
		KdJsonUtility::GetValue(jsonObj, "m_beta", &m_beta);
		KdJsonUtility::GetValue(jsonObj, "m_epsilon", &m_epsilon);
		KdJsonUtility::GetValue(jsonObj, "m_lumin", &m_lumin);
		KdJsonUtility::GetValue(jsonObj, "m_dispersion", &m_dispersion);

	}

	// このクラスの内容をJSONデータ化する
	void Serialize(nlohmann::json& outJson) const override
	{
		KdPostProcessBase::Serialize(outJson);

		outJson["m_sampleCount"] = m_sampleCount;
		outJson["m_radius"] = m_radius;
		outJson["m_sigma"] = m_sigma;
		outJson["m_beta"] = m_beta;
		outJson["m_epsilon"] = m_epsilon;
		outJson["m_lumin"] = m_lumin;
		outJson["m_dispersion"] = m_dispersion;
	}

	// ImGui
	void Editor_ImGui() override;

private:
	enum
	{
		kPass_SSAO = 0,
		kPass_BilateralBlur = 1,
	};

	// 設定
	int m_sampleCount = 9;
	float m_radius = 0.3f;
	float m_sigma = 0.1f;
	float m_beta = 0.001f;
	float m_epsilon = 0.001f;

	float m_lumin = 2.0f;
	float m_dispersion = 2.0f;

	bool m_DEBUG_ShowSSAOOnly = false;

	//
	std::shared_ptr<KdShader>			m_shader;

	std::shared_ptr<KdMaterial>			m_material;

	std::shared_ptr<KdRenderTexture>	m_rt[2];
};