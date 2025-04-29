#pragma once

#include "KdComponent.h"

#include "Math/KdMath.h"

//=========================================================
// 
//=========================================================
class KdLightComponent : public KdComponent
{
public:

	enum LightTypes
	{
		Directional,
		Spot,
		Point,
	};

	LightTypes GetLightType() const { return m_lightType; }
	bool GetCastShadow() const { return m_castShadow; }

	void SetLightColor(const Math::Color& color) { m_color = color; }

	virtual void Update() override;

	void WriteLightDataToCBuffer();

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		// �p�����̊֐������s
		KdComponent::Deserialize(jsonObj);

		KdJsonUtility::GetArray(jsonObj, "Color", &m_color.x, 3);

	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(nlohmann::json& outJson) const
	{
		// �p�����̊֐������s
		KdComponent::Serialize(outJson);

		outJson["Color"] = KdJsonUtility::CreateArray(&m_color.x, 3);
	}

	// ImGui
	virtual void Editor_ImGui() override;

private:

	LightTypes			m_lightType;

	bool				m_castShadow = true;

	Math::Color			m_color = { 1,1,1,1 };

};
