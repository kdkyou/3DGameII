/*
#include "KdGameObjectCollection.h"

#include "KdFramework.h"

#include "Editor/KdEditorData.h"
*/

/*
void KdGameObjectCollection::LoadMesh(const std::string& assetPath, std::shared_ptr<KdGameObject> parentObj)
{
	std::shared_ptr<KdModel> model = std::make_shared<KdModel>();

	if (model->Load(assetPath) == false)
	{
		return;
	}

	if (parentObj == nullptr)
	{
		parentObj = m_rootObject;
	}

	// 選択
	KdEditorData::GetInstance().SelectObj.reset();

	//----------------------------------------------------------------
	// ルートノードから全ノードにアクセスし、GameObjectを作成していく
	//----------------------------------------------------------------

	std::function<void(const KdModel::Node&, std::shared_ptr<KdGameObject>) > RecNode = [this, &model, &RecNode, &assetPath](const KdModel::Node& node, std::shared_ptr<KdGameObject> parent)
		{
			// GameObject作成
			auto go = std::make_shared<KdGameObject>();
			go->Initialize(node.NodeName);
			go->SetParent(parent, false);

			// 行列セット
			go->GetTransform()->SetLocalMatrix(node.LocalTransform.GetMatrix());

			// 選択
			if (KdEditorData::GetInstance().SelectObj.expired())
			{
				KdEditorData::GetInstance().SelectObj = go;
			}

			// メッシュレンダラー作成
			if (node.Mesh != nullptr)
			{
				auto meshRenderer = go->AddComponent<KdMeshRenderer>();

				// メッシュとマテリアルをセット
				meshRenderer->SetModel(model, node.NodeName);
			}

			// 子再帰
			for (size_t iChild = 0; iChild < node.Children.size(); iChild++)
			{
				RecNode(model->GetAllNodes()[node.Children[iChild]], go);
			}
		};

	// ルートノードから全ノードにアクセスし、GameObjectを作成していく
	RecNode(model->GetAllNodes()[0], parentObj);
}
*/

/*
void KdGameObjectCollection::Editor_ImGui()
{
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
*/