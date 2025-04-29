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

	// 環境光
	KdVector3	m_ambientColor;

	// IBLテクスチャ
	std::shared_ptr<KdTexture> m_IBLTex;

	// Fog
	KdVector3		m_distanceFogColor;
	float			m_distanceFogDensity = 0;

	// ポストプロセスリスト
	std::vector<std::shared_ptr<KdPostProcessBase>> m_postProcesses;

	void SetToDevice();

	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSONデータから、クラスの内容を設定
	void Deserialize(const nlohmann::json& jsonObj);
	// このクラスの内容をJSONデータ化する
	void Serialize(nlohmann::json& outJson) const;

	//===============================
	// Editor
	//===============================
	void Editor_ImGui();

//-------------------------------
// シングルトン
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
// ポストプロセス基本シェーダー
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
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		KdJsonUtility::GetValue(jsonObj, "Enable", &m_enable);
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(nlohmann::json& outJson) const
	{
		outJson["Name"] = GetName();
		outJson["Enable"] = m_enable;
	}

	// ImGui
	virtual void Editor_ImGui();

};