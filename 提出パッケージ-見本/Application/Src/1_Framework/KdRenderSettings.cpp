#include "KdRenderSettings.h"

#include "KdFramework.h"

#include "Editor/KdEditorData.h"

void KdRenderSettings::SetToDevice()
{

	// IBLテクスチャセット
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::IBL, m_IBLTex.get());


}

void KdRenderSettings::Deserialize(const nlohmann::json& jsonObj)
{
	// Ambient
	KdJsonUtility::GetArray(jsonObj, "AmbientColor", &m_ambientColor.x, 3);

	// IBL
	std::string IBLFilepath;
	KdJsonUtility::GetValue(jsonObj, "IBLTex", &IBLFilepath);
	if (IBLFilepath.empty() == false)
	{
		m_IBLTex = KdResourceManager::GetInstance().LoadAsset<KdTexture>(IBLFilepath);
	}

	KdJsonUtility::GetArray(jsonObj, "DistanceFogColor", &m_distanceFogColor.x, 4);
	KdJsonUtility::GetValue(jsonObj, "DistanceFogDensity", &m_distanceFogDensity);

	// PostProcess設定
	KdJsonUtility::GetArray(jsonObj, "PostProcessSettings", [this](int idx, nlohmann::json js)
		{
			std::string ppName;
			KdJsonUtility::GetValue(js, "Name", &ppName);

			// 対象のPPクラスの設定復元
			for (auto&& pp : m_postProcesses)
			{
				if (pp->GetName() == ppName)
				{
					pp->Deserialize(js);
					break;
				}
			}
		});
}

void KdRenderSettings::Serialize(nlohmann::json& outJson) const
{
	// Ambient
	outJson["AmbientColor"] = KdJsonUtility::CreateArray(&m_ambientColor.x, 3);

	// IBL
	if (m_IBLTex != nullptr)
	{
		outJson["IBLTex"] = m_IBLTex->GetFilepath();
	}
	else
	{
		outJson["IBLTex"] = "";
	}

	outJson["DistanceFogColor"] = KdJsonUtility::CreateArray(&m_distanceFogColor.x, 4);
	outJson["DistanceFogDensity"] = m_distanceFogDensity;

	// PostProcess設定
	auto ppAry = nlohmann::json::array();
	for (auto&& pp : m_postProcesses)
	{
		auto jsObj = nlohmann::json::object();
		pp->Serialize(jsObj);

		ppAry.push_back(jsObj);
	}
	outJson["PostProcessSettings"] = ppAry;
}

void KdRenderSettings::Editor_ImGui()
{
	// 全シェーダー
	if (ImGui::CollapsingHeader(u8"全シェーダー", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		// 通常シェーダー
		for (auto&& pair : KdShaderManager::GetInstance().GetShaderMap())
		{
			auto shader = pair.second;

			ImGui::PushID(shader.get());

			if (ImGui::CollapsingHeader(pair.first.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// シェーダー再読み込み
				if (ImGui::Button(u8"再読み込み"))
				{
					shader->ReloadShader();
				}
			}

			ImGui::PopID();
		}

		// ポストプロセスシェーダー
		for (auto&& pp : KdRenderSettings::GetInstance().m_postProcesses)
		{
			ImGui::PushID(pp.get());

			if (ImGui::CollapsingHeader(pp->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				pp->Editor_ImGui();

				// シェーダー再読み込み
				if (ImGui::Button(u8"再読み込み"))
				{
					pp->Initialize();
				}
			}

			ImGui::PopID();
		}

		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader(u8"環境", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		// 環境光
		ImGui::DragFloat3("Ambient Color", &m_ambientColor.x, 0.01f);

		// IBL
		if (m_IBLTex != nullptr)
		{
			ImGui::Text(u8"IBLテクスチャ(あり)");

			ImGui::Image((ImTextureID)m_IBLTex->GetSRViewArray()[0].Get(), ImVec2(50, 50));
			ImGui::SameLine();
			ImGui::Image((ImTextureID)m_IBLTex->GetSRViewArray()[1].Get(), ImVec2(50, 50));
			ImGui::SameLine();
			ImGui::Image((ImTextureID)m_IBLTex->GetSRViewArray()[2].Get(), ImVec2(50, 50));

			ImGui::Image((ImTextureID)m_IBLTex->GetSRViewArray()[3].Get(), ImVec2(50, 50));
			ImGui::SameLine();
			ImGui::Image((ImTextureID)m_IBLTex->GetSRViewArray()[4].Get(), ImVec2(50, 50));
			ImGui::SameLine();
			ImGui::Image((ImTextureID)m_IBLTex->GetSRViewArray()[5].Get(), ImVec2(50, 50));

			if (ImGui::Button(u8"IBlテクスチャ解除"))
			{
				m_IBLTex = nullptr;
			}
		}
		else
		{
			ImGui::Text(u8"IBLテクスチャ(なし)");
		}

		if (ImGui::Button(u8"IBlテクスチャ読み込み"))
		{
			std::string filepath;
			if (KdEditorData::GetInstance().OpenFileDialog(filepath, "IBLテクスチャを開く", "DDSファイル\0*.dds\0"))
			{
				auto tex = KdResourceManager::GetInstance().LoadAsset<KdTexture>(filepath);
				m_IBLTex = tex;
			}

		}

		// フォグ
		ImGui::DragFloat(u8"距離フォグ密度", &m_distanceFogDensity, 0.001f);
		ImGui::ColorEdit3(u8"距離フォグ色", &m_distanceFogColor.x);

		// 
		ImGui::Unindent();
	}
}
