#pragma once

#include "KdFramework.h"

class PP_ToneMapping : public KdPostProcessBase
{
public:

	virtual std::string GetName() const override { return "ToneMapping"; }

	virtual Timings GetTiming() override { return Timings::AfterTransparent; }
	virtual int32_t GetPriority() override { return 0; }

	void Initialize() override;

	virtual void Execute(KdScreenData& screenData) override;

private:

	std::shared_ptr<KdShader>		m_shader;
	std::shared_ptr<KdMaterial>		m_material;
};