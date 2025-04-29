#include "KdRenderSettings.h"

#include "KdFramework.h"

#include "Editor/KdEditorData.h"

void KdRenderSettings::SetToDevice()
{

	// IBL�e�N�X�`���Z�b�g
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

	// PostProcess�ݒ�
	KdJsonUtility::GetArray(jsonObj, "PostProcessSettings", [this](int idx, nlohmann::json js)
		{
			std::string ppName;
			KdJsonUtility::GetValue(js, "Name", &ppName);

			// �Ώۂ�PP�N���X�̐ݒ蕜��
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

	// PostProcess�ݒ�
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
	// �S�V�F�[�_�[
	if (ImGui::CollapsingHeader(u8"�S�V�F�[�_�[", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		// �ʏ�V�F�[�_�[
		for (auto&& pair : KdShaderManager::GetInstance().GetShaderMap())
		{
			auto shader = pair.second;

			ImGui::PushID(shader.get());

			if (ImGui::CollapsingHeader(pair.first.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// �V�F�[�_�[�ēǂݍ���
				if (ImGui::Button(u8"�ēǂݍ���"))
				{
					shader->ReloadShader();
				}
			}

			ImGui::PopID();
		}

		// �|�X�g�v���Z�X�V�F�[�_�[
		for (auto&& pp : KdRenderSettings::GetInstance().m_postProcesses)
		{
			ImGui::PushID(pp.get());

			if (ImGui::CollapsingHeader(pp->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				pp->Editor_ImGui();

				// �V�F�[�_�[�ēǂݍ���
				if (ImGui::Button(u8"�ēǂݍ���"))
				{
					pp->Initialize();
				}
			}

			ImGui::PopID();
		}

		ImGui::Unindent();
	}

	if (ImGui::CollapsingHeader(u8"��", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Indent();

		// ����
		ImGui::DragFloat3("Ambient Color", &m_ambientColor.x, 0.01f);

		// IBL
		if (m_IBLTex != nullptr)
		{
			ImGui::Text(u8"IBL�e�N�X�`��(����)");

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

			if (ImGui::Button(u8"IBl�e�N�X�`������"))
			{
				m_IBLTex = nullptr;
			}
		}
		else
		{
			ImGui::Text(u8"IBL�e�N�X�`��(�Ȃ�)");
		}

		if (ImGui::Button(u8"IBl�e�N�X�`���ǂݍ���"))
		{
			std::string filepath;
			if (KdEditorData::GetInstance().OpenFileDialog(filepath, "IBL�e�N�X�`�����J��", "DDS�t�@�C��\0*.dds\0"))
			{
				auto tex = KdResourceManager::GetInstance().LoadAsset<KdTexture>(filepath);
				m_IBLTex = tex;
			}

		}

		// �t�H�O
		ImGui::DragFloat(u8"�����t�H�O���x", &m_distanceFogDensity, 0.001f);
		ImGui::ColorEdit3(u8"�����t�H�O�F", &m_distanceFogColor.x);

		// 
		ImGui::Unindent();
	}
}
