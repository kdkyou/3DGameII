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
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		// �p�����̊֐������s
		KdComponent::Deserialize(jsonObj);

		KdJsonUtility::GetValue(jsonObj, "FOV", &m_proj_FOV);
		KdJsonUtility::GetArray(jsonObj, "FearAndFar", &m_proj_NearAndFar.x, 2);

	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(nlohmann::json& outJson) const
	{
		// �p�����̊֐������s
		KdComponent::Serialize(outJson);

		outJson["FOV"] = m_proj_FOV;
		outJson["FearAndFar"] = KdJsonUtility::CreateArray(&m_proj_NearAndFar.x, 2);
	}

	virtual void Editor_ImGui() override;

private:

	// �ˉe�s��p�f�[�^
	float			m_proj_FOV = 60;						// ����p
	KdVector2		m_proj_NearAndFar = { 0.01f, 5000 };	// �ŋߐڋ����ƍŉ�������


	// �`�����
	std::shared_ptr<KdScreenData> m_screenData;
};
