#pragma once

#include "CoreSystem.h"

#include "KdScreenData.h"

class KdPostProcessBase;

//=========================================================
// 
// 
// 
//=========================================================
class KdRenderSettings
{
public:

	// ����
	KdVector3	m_ambientColor;

	// IBL�e�N�X�`��
	std::shared_ptr<KdTexture> m_IBLTex;

	// Fog
	KdVector3		m_distanceFogColor;
	float			m_distanceFogDensity = 0;

	// �|�X�g�v���Z�X���X�g
	std::vector<std::shared_ptr<KdPostProcessBase>> m_postProcesses;

	void SetToDevice();

	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	void Deserialize(const nlohmann::json& jsonObj);
	// ���̃N���X�̓��e��JSON�f�[�^������
	void Serialize(nlohmann::json& outJson) const;

	//===============================
	// Editor
	//===============================
	void Editor_ImGui();

//-------------------------------
// �V���O���g��
//-------------------------------
private:

	KdRenderSettings()
	{
	}

public:
	static KdRenderSettings& GetInstance() {
		static KdRenderSettings instance;
		return instance;
	}

};

//=========================================================
// �|�X�g�v���Z�X��{�V�F�[�_�[
//=========================================================
class KdPostProcessBase
{
public:

	virtual std::string GetName() const = 0;

	bool			m_enable = true;

	enum Timings
	{
		AfterOpaque,
		AfterTransparent,
	};

	virtual Timings GetTiming() = 0;
	virtual int32_t GetPriority() = 0;
	virtual void Initialize() {}
	virtual void Execute(KdScreenData& screenData) = 0;


	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		KdJsonUtility::GetValue(jsonObj, "Enable", &m_enable);
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(nlohmann::json& outJson) const
	{
		outJson["Name"] = GetName();
		outJson["Enable"] = m_enable;
	}

	// ImGui
	virtual void Editor_ImGui();

};