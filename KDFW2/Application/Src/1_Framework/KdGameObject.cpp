#include "KdGameObject.h"

#include "CoreSystem.h"
#include "KdFramework.h"

#include "Component/KdComponent.h"
#include "Component/KdTransformComponent.h"



void KdGameObject::Initialize(const std::string& name)
{
	m_name = name;

	// Transform���Ȃ��ꍇ�͍쐬
	if (m_components.size() == 0)
	{
		AddComponent<KdTransformComponent>();
	}
}


void KdGameObject::Collect(std::vector<KdGameObject*>& gameObjectList)
{
	// �������͑ΏۊO
	if (m_enable == false) return;

	// �����Ώۂɓo�^����
	gameObjectList.push_back(this);

	// �q���ċA�Ŏ��s���Ă���
	for (auto it = m_children.begin(); it != m_children.end(); )
	{
		// �폜�t���O��ON?
		if ((*it)->IsDestroy())
		{
			// ���X�g�������
			it = m_children.erase(it);
			continue;
		}

		// Start

		// �o�^
		(*it)->Collect(gameObjectList);

		// ����
		++it;
	}
}

std::shared_ptr<KdComponent> KdGameObject::AddComponent(const std::string& className)
{
	// �쐬
	auto component = KdClassAssembly::MakeShared<KdComponent>(className);

	AddComponent(component);

	return component;
}

void KdGameObject::AddComponent(const std::shared_ptr<KdComponent>& comp)
{
	auto gObj = std::static_pointer_cast<KdGameObject>(shared_from_this());
	// ������������Ƃ��ăZ�b�g
	comp->SetGameObject(gObj);
	// ���X�g�ɒǉ�
	m_components.push_back(comp);
}

void KdGameObject::SetParent(const std::shared_ptr<KdGameObject>& newParent, bool worldPositionStays)
{
	// ���݂̃��[���h�s��
	KdMatrix mW = GetTransform()->GetWorldMatrix();

	// �e����E�ނ���O�ɁA������shared_ptr���L��
	std::shared_ptr<KdGameObject> spThis = ToSharedPtr(this);

	// ���݂̐eGameObject����E�ނ���
	auto nowParent = GetParent();
	if (nowParent)
	{
		// ����
		auto it = std::find(nowParent->m_children.begin(), nowParent->m_children.end(), spThis);
		// ����
		if (it != nowParent->m_children.end())
		{
			// �폜
			nowParent->m_children.erase(it);
		}
	}

	// �e�|�C���^��ύX
	m_parent = newParent;

	GetTransform()->SetChanged();

	// �V�����e�̎q�Ƃ��āA������o�^
	if (newParent)
	{
		newParent->m_children.push_back(spThis);

		// ���[���h�s����ێ�����
		if (worldPositionStays)
		{
			GetTransform()->SetWorldMatrix(mW);
		}
	}
}

void KdGameObject::Deserialize(const nlohmann::json& jsonObj)
{
	std::string strGuid;
	KdJsonUtility::GetValue(jsonObj, "Guid", &strGuid);
	m_guid.FromString(strGuid);

	KdJsonUtility::GetValue(jsonObj, "Enable", &m_enable);
	KdJsonUtility::GetValue(jsonObj, "Name", &m_name);

	KdJsonUtility::GetValue(jsonObj, "Tag", &m_tag);

	//-------------------------
	// �S�R���|�[�l���g
	//-------------------------
	m_components.clear();
	auto& compAry = jsonObj["Components"];
	for (auto&& compJson : compAry)
	{
		// �N���X������R���|�[�l���g�ǉ�
		auto comp = AddComponent(compJson["ClassName"].get<std::string>());
		if (comp != nullptr)
		{
			// JSON�f�[�^����ݒ�
			comp->Deserialize(compJson);
		}
	}

	//-------------------------
	// �S�Ă̎q
	//-------------------------
	m_children.clear();
	auto& childAry = jsonObj["Children"];
	for (auto&& childJson : childAry)
	{
		// �V�KGameObject���쐬���A�f�V���A���C�Y
		std::shared_ptr<KdGameObject> newObj = std::make_shared<KdGameObject>();
		newObj->Deserialize(childJson);

		// �q���X�g�֒ǉ�
		m_children.push_back(newObj);
		// ������e�Ƃ��Đݒ�
		newObj->m_parent = ToSharedPtr(this);
	}
}

void KdGameObject::Serialize(nlohmann::json& outJson) const
{
	outJson["Guid"] = m_guid.ToString();
	outJson["Enable"] = m_enable;
	outJson["Name"] = m_name;
	outJson["Tag"] = m_tag;

	//-------------------------
	// �S�R���|�[�l���g
	//-------------------------
	// �S�R���|�[�l���g������z��
	nlohmann::json compAry = nlohmann::json::array();
	// �R���|�[�l���g���V���A���C�Y���A�z��֒ǉ����Ă���
	for (auto&& comp : m_components)
	{
		nlohmann::json compSerial = nlohmann::json::object();
		comp->Serialize(compSerial);

		// �z��֒ǉ�
		compAry.push_back(compSerial);
	}

	outJson["Components"] = compAry;

	//-------------------------
	// �q���ċA���s
	//-------------------------
	// �S�Ă̎q������z��
	nlohmann::json childAry = nlohmann::json::array();
	for (auto&& child : m_children)
	{
		nlohmann::json objSerial = nlohmann::json::object();
		child->Serialize(objSerial);

		// �z��֒ǉ�
		childAry.push_back(objSerial);
	}

	outJson["Children"] = childAry;
}

void KdGameObject::Editor_ImGui()
{
	ImGui::Text(m_guid.ToString().c_str());

	// �L���t���O
	ImGui::Checkbox("Enable", &m_enable);
	// ���O
	ImGui::InputText("m_name", &m_name);
	// Tag
	ImGui::InputText("m_tag", &m_tag);

	//-------------------------------
	// �S�R���|�[�l���g
	//-------------------------------
	for(size_t iComp = 0;iComp < m_components.size(); iComp++)
//	for (auto& it = m_components.begin(); it != m_components.end(); )
	{
		auto& comp = m_components[iComp];

		ImGui::PushID(comp.get());

		// �R���|�[�l���g��
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.5f, 0.2f, 0.2f, 1));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.8f, 0.2f, 0.2f, 1));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 0.2f, 0.2f, 1));

		bool bOpen = ImGui::CollapsingHeader(comp->GetClassName().c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::PopStyleColor(3);

		// ����
		//ImGuiShowHelp(comp->Editor_GetComment().c_str());

		/*
		// ���j���[
		if (ImGui::BeginPopupContextItem("ComponentMenuPopup"))
		{
			if (ImGui::Selectable(u8"�P���"))
			{
				GoPrevSiblingComponent(comp);
				bOpen = false;
			}

			if (ImGui::Selectable(u8"�P����"))
			{
				GoNextSiblingComponent(comp);
				bOpen = false;
			}

			ImGui::Separator();
			{
				static json11::Json::object s_copyComponent;
				if (ImGui::Selectable(u8"�R�s�["))
				{
					comp->Serialize(s_copyComponent);
				}
				if (ImGui::Selectable(u8"�\��t��"))
				{
					if (comp->GetClassName() == s_copyComponent["ClassName"].string_value())
					{
						comp->Deserialize(s_copyComponent);
						bOpen = false;
					}
				}
			}
			ImGui::Separator();

			if (ImGui::Selectable(u8"�폜"))
			{
				it = m_components.erase(it);
				bOpen = false;
			}

			ImGui::EndPopup();
		}
		*/

		// ���e
		if (bOpen)
		{
			ImGui::Indent();
			comp->Editor_ImGui();
			ImGui::Unindent();
		}

		ImGui::PopID();

//		if (it == m_components.end())break;
//		++it;
	}
}
