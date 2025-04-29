#include "KdScene.h"

#include "KdFramework.h"

#include "Editor/KdEditorData.h"

void KdScene::Deserialize(const nlohmann::json& jsonObj)
{
	//-------------------
	// GameObject
	//-------------------
	m_rootObject = std::make_shared<KdGameObject>();
	m_rootObject->Deserialize(jsonObj["RootGameObject"]);
	/*
	KdJsonUtility::GetValue(jsonObj, "RootGameObject", [this](nlohmann::json js)
		{
			m_rootObject->Deserialize(js);
		});
	*/

	//-------------------
	// RenderSettings
	//-------------------
	KdJsonUtility::GetValue(jsonObj, "RenderSettings", [this](nlohmann::json js)
		{
			KdRenderSettings::GetInstance().Deserialize(js);
		});
}

void KdScene::Serialize(nlohmann::json& outJson) const
{
	nlohmann::json serialGameObj = nlohmann::json::object();

	//-------------------
	// GameObject
	//-------------------
	m_rootObject->Serialize(serialGameObj);
	outJson["RootGameObject"] = serialGameObj;

	//-------------------
	// RenderSettings
	//-------------------
	nlohmann::json serialRenderSettings = nlohmann::json::object();
	KdRenderSettings::GetInstance().Serialize(serialRenderSettings);
	outJson["RenderSettings"] = serialRenderSettings;
}

bool KdScene::LoadFromFile(const std::string& filepath)
{
	Release();

	nlohmann::json serial;
	std::ifstream ifs(filepath);
	if (!ifs)return false;

	// �ǂݍ���
	ifs >> serial;

	// �f�V���A���C�Y
	Deserialize(serial);

	return true;
}

bool KdScene::SaveToFile(const std::string& filepath)
{
	std::ofstream ofs(filepath);
	if (!ofs) return false;

	// �V���A���C�Y
	nlohmann::json serial = nlohmann::json::object();
	Serialize(serial);

	// ������
	std::string str = serial.dump();

	// �t�@�C����������
	ofs.write(str.c_str(), str.size());

	return true;
}

void KdScene::Editor_ImGui()
{
	if (ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_MenuBar))
	{
		// ���j���[ 
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(u8"�t�@�C��"))
			{
				// Load���j���[ 
				if (ImGui::MenuItem(u8"�V�[���ǂݍ���"))
				{
					std::string filepath;
					if (KdEditorData::GetInstance().OpenFileDialog(filepath, "�V�[���t�@�C���J��", "�V�[���t�@�C��\0*.scene\0"))
					{
						std::shared_ptr<KdScene> scene = std::make_shared<KdScene>();

						Release();

						if (LoadFromFile(filepath))
						{
							KdEditorData::GetInstance().m_logWindow.AddLog(u8"�V�[���ǂݍ��݊���");
						}
					}
				}

				// Save���j���[ 
				if (ImGui::MenuItem(u8"�V�[���ۑ�"))
				{
					std::string filepath;
					if (KdEditorData::GetInstance().SaveFileDialog(filepath, "�V�[���t�@�C���ۑ�", "�V�[���t�@�C��\0*.scene\0"))
					{
						SaveToFile(filepath);
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// ���ݑI������Ă���GameObject
		auto nowSelectedObj = KdEditorData::GetInstance().SelectObj.lock();

		std::function<void(std::shared_ptr<KdGameObject>)> rec = [this, nowSelectedObj, &rec](std::shared_ptr<KdGameObject> gameObj)
			{
				ImGui::PushID(gameObj.get());

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

				// �I�𒆁H
				if (nowSelectedObj == gameObj)
				{
					flags |= ImGuiTreeNodeFlags_Selected;
				}

				// ����
				bool bOpen = ImGui::TreeNodeEx(gameObj->GetName().c_str(), flags);

				//-----------------
				// �N���b�N��
				//-----------------
				if (ImGui::IsItemClicked())
				{
					KdEditorData::GetInstance().SelectObj = gameObj;

					nlohmann::json serial = nlohmann::json::object();
					gameObj->Serialize(serial);

					int a = 0;
				}

				//-----------------
				// �E�N���b�N
				//-----------------
				if (ImGui::BeginPopupContextItem("GameObjectPopup"))
				{
					// ��̃Q�[���I�u�W�F�N�g
					if (ImGui::Selectable(u8"���GameObject�ǉ�"))
					{
						auto newObj = std::make_shared<KdGameObject>();
						newObj->Initialize("New GameObject");

						newObj->SetParent(gameObj, false);

						// �I��
						KdEditorData::GetInstance().SelectObj = newObj;
					}

					ImGui::Separator();

					// Prefab�ۑ�
					if (ImGui::Selectable(u8"Prefab�t�@�C���ۑ�"))
					{

					}

					ImGui::Separator();

					// ����
					if (ImGui::Selectable(u8"����"))
					{
						// �V���A���C�Y
						nlohmann::json serial = nlohmann::json::object();
						gameObj->Serialize(serial);

						// GameObject�쐬
						auto newObj = std::make_shared<KdGameObject>();
						// JSON�f�[�^����ݒ�(�f�V���A���C�Y)
						newObj->Deserialize(serial);
						// �ǉ�
						newObj->SetParent(gameObj->GetParent(), false);

						// �I��
						KdEditorData::GetInstance().SelectObj = newObj;
					}

					ImGui::Separator();

					// �폜
					if (ImGui::Selectable(u8"�폜"))
					{
						gameObj->Destroy();
					}

					// 

					ImGui::EndPopup();
				}

				//-----------------
				// �q
				//-----------------
				if (bOpen)
				{
					// �q
					for (auto&& child : gameObj->GetChildren())
					{
						rec(child);
					}

					ImGui::TreePop();
				}

				ImGui::PopID();
			};

		rec(m_rootObject);

	}
	ImGui::End();
}
