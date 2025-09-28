#pragma once


class PP_ColorCollection :public KdPostProcessBase {
public:

	enum effectPass
	{
		GrayScale,
		Sepia,
		TwoGradiation,
		Vignette
	};

	virtual std::string GetName() const override { return "ColorCollection"; }

	// このPPを実行するタイミング
	virtual Timings GetTiming()override { return Timings::AfterTransparent; }
	virtual int32_t GetPriority()override { return 0; }

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
	}

	// このクラスの内容をJSONデータ化する
	void Serialize(nlohmann::json& outJson) const override
	{
		KdPostProcessBase::Serialize(outJson);
	}

	// ImGui
	void Editor_ImGui() override;

private:
	// 設定

	bool m_isGrayscale = false;
	bool m_isSepia = false;
	bool m_isTwoGradiation = false;
	bool m_isVignette = false;

	float m_twoGradiationValue = 0.4f;
	float m_vignetteIntenPow = 0.8f;
	float m_vignetteMaskPow = 0.5f;
	Math::Vector2 m_vignetteCenter = { 0.5f,0.5f };

	// ビネット表現に村をつける模様テクスチャ
	std::shared_ptr<KdTexture> m_vignetteMask = nullptr;
	
	// シェーダー本体
	std::shared_ptr<KdShader>			m_shader;
	std::shared_ptr<KdMaterial>			m_material;


};