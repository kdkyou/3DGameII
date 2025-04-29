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

	// �I��
	KdEditorData::GetInstance().SelectObj.reset();

	//----------------------------------------------------------------
	// ���[�g�m�[�h����S�m�[�h�ɃA�N�Z�X���AGameObject���쐬���Ă���
	//----------------------------------------------------------------

	std::function<void(const KdModel::Node&, std::shared_ptr<KdGameObject>) > RecNode = [this, &model, &RecNode, &assetPath](const KdModel::Node& node, std::shared_ptr<KdGameObject> parent)
		{
			// GameObject�쐬
			auto go = std::make_shared<KdGameObject>();
			go->Initialize(node.NodeName);
			go->SetParent(parent, false);

			// �s��Z�b�g
			go->GetTransform()->SetLocalMatrix(node.LocalTransform.GetMatrix());

			// �I��
			if (KdEditorData::GetInstance().SelectObj.expired())
			{
				KdEditorData::GetInstance().SelectObj = go;
			}

			// ���b�V�������_���[�쐬
			if (node.Mesh != nullptr)
			{
				auto meshRenderer = go->AddComponent<KdMeshRenderer>();

				// ���b�V���ƃ}�e���A�����Z�b�g
				meshRenderer->SetModel(model, node.NodeName);
			}

			// �q�ċA
			for (size_t iChild = 0; iChild < node.Children.size(); iChild++)
			{
				RecNode(model->GetAllNodes()[node.Children[iChild]], go);
			}
		};

	// ���[�g�m�[�h����S�m�[�h�ɃA�N�Z�X���AGameObject���쐬���Ă���
	RecNode(model->GetAllNodes()[0], parentObj);
}
*/

/*
void KdGameObjectCollection::Editor_ImGui()
{
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
*/