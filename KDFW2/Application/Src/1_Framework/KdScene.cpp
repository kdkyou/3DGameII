#include "KdScene.h"

#include "KdFramework.h"

#include "Editor/KdEditorData.h"

std::shared_ptr<KdGameObject> KdScene::Instantiate(const std::string& prefabFilePath, std::shared_ptr<KdGameObject> parent)
{
	std::shared_ptr<KdGameObject> newObject = nullptr;

	auto newParent = parent;
	if (newParent == nullptr)
	{
		//�w�肪�Ȃ����Root��e�ɂ���
		newParent = m_rootObject;
	}

	//�v���n�u�t�@�C���̓Ǎ�
	//ResourceManager�Ɉڍs�\��
	std::ifstream ifs (prefabFilePath);
	if (ifs.fail() == false)
	{
		//�ǂݍ��񂾃f�[�^��Json�f�[�^�ɓW�J
		nlohmann::json serial;
		ifs >> serial;

		//GameObject�쐬
		newObject = std::make_shared<KdGameObject>();
		newObject->Deserialize(serial);

		//�e�̎w��
		newObject->SetParent(newParent, false);
	}

	return newObject;
}

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
						//�I�����Ă���GameObject�̃V���A���C�Y
						nlohmann::json serial = nlohmann::json::object();
						gameObj->Serialize(serial);

						//�t�@�C���Ƃ��ďo��
						std::string saveFilePath = "";
						if (KdEditorData::GetInstance().SaveFileDialog(saveFilePath, 
										"Prefab�t�@�C���ۑ�", "Prefab�t�@�C��\0*.kdprefab\0"))
						{
							std::ofstream ofs(saveFilePath);
							//���s���Ă����true���A���Ă���
							if (ofs.fail() == false) 
							{
								//Json�𕶎���Ƃ��ĕۑ�s
								std::string str = serial.dump();
								ofs.write ( str.c_str(), str.size() );
							}
							ofs.close();
						}

					}

					//Prefab�ǂݍ���
					if (ImGui::Selectable(u8"Prefab�t�@�C���Ǎ�"))
					{
						
						std::string openFilePath = "";
						if (KdEditorData::GetInstance().OpenFileDialog(
							openFilePath,"Prefab�t�@�C�����J��","Prefab�t�@�C��\0*.kdprefab\0" ) )
						{
							//�t�@�C�����I�����ꂽ
							int i = 0;
							std::ifstream ifs(openFilePath);
							if (ifs.fail() == false)
							{
								nlohmann::json serial;
								ifs >> serial;	//�����񂩂�ǂݍ����Json�f�[�^�ɓW�J

								//�W�J��͐V����GameObject
								auto newObj = std::make_shared<KdGameObject>();
								newObj->Deserialize(serial);

								//�I��(�E�N���b�N)���ꂽ�I�u�W�F�N�g��e�ɂ���
								newObj->SetParent(gameObj, false);

								//�V�����ł���GameObject��I������
								KdEditorData::GetInstance().SelectObj = newObj;
							}
						}
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
