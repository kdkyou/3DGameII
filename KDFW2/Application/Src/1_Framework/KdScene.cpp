#include "KdScene.h"

#include "KdFramework.h"

#include "Editor/KdEditorData.h"

std::shared_ptr<KdGameObject> KdScene::Instantiate(const std::string& prefabFilePath, std::shared_ptr<KdGameObject> parent)
{
	std::shared_ptr<KdGameObject> newObject = nullptr;

	auto newParent = parent;
	if (newParent == nullptr)
	{
		//指定がなければRootを親にする
		newParent = m_rootObject;
	}

	//プレハブファイルの読込
	//ResourceManagerに移行予定
	std::ifstream ifs (prefabFilePath);
	if (ifs.fail() == false)
	{
		//読み込んだデータをJsonデータに展開
		nlohmann::json serial;
		ifs >> serial;

		//GameObject作成
		newObject = std::make_shared<KdGameObject>();
		newObject->Deserialize(serial);

		//親の指定
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

	// 読み込み
	ifs >> serial;

	// デシリアライズ
	Deserialize(serial);

	return true;
}

bool KdScene::SaveToFile(const std::string& filepath)
{
	std::ofstream ofs(filepath);
	if (!ofs) return false;

	// シリアライズ
	nlohmann::json serial = nlohmann::json::object();
	Serialize(serial);

	// 文字列化
	std::string str = serial.dump();

	// ファイル書き込み
	ofs.write(str.c_str(), str.size());

	return true;
}

void KdScene::Editor_ImGui()
{
	if (ImGui::Begin("Hierarchy", 0, ImGuiWindowFlags_MenuBar))
	{
		// メニュー 
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu(u8"ファイル"))
			{
				// Loadメニュー 
				if (ImGui::MenuItem(u8"シーン読み込み"))
				{
					std::string filepath;
					if (KdEditorData::GetInstance().OpenFileDialog(filepath, "シーンファイル開く", "シーンファイル\0*.scene\0"))
					{
						std::shared_ptr<KdScene> scene = std::make_shared<KdScene>();

						Release();

						if (LoadFromFile(filepath))
						{
							KdEditorData::GetInstance().m_logWindow.AddLog(u8"シーン読み込み完了");
						}
					}
				}

				// Saveメニュー 
				if (ImGui::MenuItem(u8"シーン保存"))
				{
					std::string filepath;
					if (KdEditorData::GetInstance().SaveFileDialog(filepath, "シーンファイル保存", "シーンファイル\0*.scene\0"))
					{
						SaveToFile(filepath);
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// 現在選択されているGameObject
		auto nowSelectedObj = KdEditorData::GetInstance().SelectObj.lock();

		std::function<void(std::shared_ptr<KdGameObject>)> rec = [this, nowSelectedObj, &rec](std::shared_ptr<KdGameObject> gameObj)
			{
				ImGui::PushID(gameObj.get());

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;

				// 選択中？
				if (nowSelectedObj == gameObj)
				{
					flags |= ImGuiTreeNodeFlags_Selected;
				}

				// 項目
				bool bOpen = ImGui::TreeNodeEx(gameObj->GetName().c_str(), flags);

				//-----------------
				// クリック時
				//-----------------
				if (ImGui::IsItemClicked())
				{
					KdEditorData::GetInstance().SelectObj = gameObj;

					nlohmann::json serial = nlohmann::json::object();
					gameObj->Serialize(serial);

					int a = 0;
				}

				//-----------------
				// 右クリック
				//-----------------
				if (ImGui::BeginPopupContextItem("GameObjectPopup"))
				{
					// 空のゲームオブジェクト
					if (ImGui::Selectable(u8"空のGameObject追加"))
					{
						auto newObj = std::make_shared<KdGameObject>();
						newObj->Initialize("New GameObject");

						newObj->SetParent(gameObj, false);

						// 選択
						KdEditorData::GetInstance().SelectObj = newObj;
					}

					ImGui::Separator();

					// Prefab保存
					if (ImGui::Selectable(u8"Prefabファイル保存"))
					{
						//選択しているGameObjectのシリアライズ
						nlohmann::json serial = nlohmann::json::object();
						gameObj->Serialize(serial);

						//ファイルとして出力
						std::string saveFilePath = "";
						if (KdEditorData::GetInstance().SaveFileDialog(saveFilePath, 
										"Prefabファイル保存", "Prefabファイル\0*.kdprefab\0"))
						{
							std::ofstream ofs(saveFilePath);
							//失敗しているとtrueが帰ってくる
							if (ofs.fail() == false) 
							{
								//Jsonを文字列として保存s
								std::string str = serial.dump();
								ofs.write ( str.c_str(), str.size() );
							}
							ofs.close();
						}

					}

					//Prefab読み込み
					if (ImGui::Selectable(u8"Prefabファイル読込"))
					{
						
						std::string openFilePath = "";
						if (KdEditorData::GetInstance().OpenFileDialog(
							openFilePath,"Prefabファイルを開く","Prefabファイル\0*.kdprefab\0" ) )
						{
							//ファイルが選択された
							int i = 0;
							std::ifstream ifs(openFilePath);
							if (ifs.fail() == false)
							{
								nlohmann::json serial;
								ifs >> serial;	//文字列から読み込んでJsonデータに展開

								//展開先は新しいGameObject
								auto newObj = std::make_shared<KdGameObject>();
								newObj->Deserialize(serial);

								//選択(右クリック)されたオブジェクトを親にする
								newObj->SetParent(gameObj, false);

								//新しくできたGameObjectを選択する
								KdEditorData::GetInstance().SelectObj = newObj;
							}
						}
					}



					ImGui::Separator();

					// 複製
					if (ImGui::Selectable(u8"複製"))
					{
						// シリアライズ
						nlohmann::json serial = nlohmann::json::object();
						gameObj->Serialize(serial);

						// GameObject作成
						auto newObj = std::make_shared<KdGameObject>();
						// JSONデータから設定(デシリアライズ)
						newObj->Deserialize(serial);
						// 追加
						newObj->SetParent(gameObj->GetParent(), false);

						// 選択
						KdEditorData::GetInstance().SelectObj = newObj;
					}

					ImGui::Separator();

					// 削除
					if (ImGui::Selectable(u8"削除"))
					{
						gameObj->Destroy();
					}

					// 

					ImGui::EndPopup();
				}

				//-----------------
				// 子
				//-----------------
				if (bOpen)
				{
					// 子
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
