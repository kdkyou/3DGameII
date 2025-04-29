#include "KdGameObject.h"

#include "CoreSystem.h"
#include "KdFramework.h"

#include "Component/KdComponent.h"
#include "Component/KdTransformComponent.h"



void KdGameObject::Initialize(const std::string& name)
{
	m_name = name;

	// Transformがない場合は作成
	if (m_components.size() == 0)
	{
		AddComponent<KdTransformComponent>();
	}
}


void KdGameObject::Collect(std::vector<KdGameObject*>& gameObjectList)
{
	// 無効時は対象外
	if (m_enable == false) return;

	// 処理対象に登録する
	gameObjectList.push_back(this);

	// 子も再帰で実行していく
	for (auto it = m_children.begin(); it != m_children.end(); )
	{
		// 削除フラグがON?
		if ((*it)->IsDestroy())
		{
			// リストから消す
			it = m_children.erase(it);
			continue;
		}

		// Start

		// 登録
		(*it)->Collect(gameObjectList);

		// 次へ
		++it;
	}
}

std::shared_ptr<KdComponent> KdGameObject::AddComponent(const std::string& className)
{
	// 作成
	auto component = KdClassAssembly::MakeShared<KdComponent>(className);

	AddComponent(component);

	return component;
}

void KdGameObject::AddComponent(const std::shared_ptr<KdComponent>& comp)
{
	auto gObj = std::static_pointer_cast<KdGameObject>(shared_from_this());
	// 自分を持ち主としてセット
	comp->SetGameObject(gObj);
	// リストに追加
	m_components.push_back(comp);
}

void KdGameObject::SetParent(const std::shared_ptr<KdGameObject>& newParent, bool worldPositionStays)
{
	// 現在のワールド行列
	KdMatrix mW = GetTransform()->GetWorldMatrix();

	// 親から脱退する前に、自分のshared_ptrを記憶
	std::shared_ptr<KdGameObject> spThis = ToSharedPtr(this);

	// 現在の親GameObjectから脱退する
	auto nowParent = GetParent();
	if (nowParent)
	{
		// 検索
		auto it = std::find(nowParent->m_children.begin(), nowParent->m_children.end(), spThis);
		// 発見
		if (it != nowParent->m_children.end())
		{
			// 削除
			nowParent->m_children.erase(it);
		}
	}

	// 親ポインタを変更
	m_parent = newParent;

	GetTransform()->SetChanged();

	// 新しい親の子として、自分を登録
	if (newParent)
	{
		newParent->m_children.push_back(spThis);

		// ワールド行列を維持する
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
	// 全コンポーネント
	//-------------------------
	m_components.clear();
	auto& compAry = jsonObj["Components"];
	for (auto&& compJson : compAry)
	{
		// クラス名からコンポーネント追加
		auto comp = AddComponent(compJson["ClassName"].get<std::string>());
		if (comp != nullptr)
		{
			// JSONデータから設定
			comp->Deserialize(compJson);
		}
	}

	//-------------------------
	// 全ての子
	//-------------------------
	m_children.clear();
	auto& childAry = jsonObj["Children"];
	for (auto&& childJson : childAry)
	{
		// 新規GameObjectを作成し、デシリアライズ
		std::shared_ptr<KdGameObject> newObj = std::make_shared<KdGameObject>();
		newObj->Deserialize(childJson);

		// 子リストへ追加
		m_children.push_back(newObj);
		// 自分を親として設定
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
	// 全コンポーネント
	//-------------------------
	// 全コンポーネントを入れる配列
	nlohmann::json compAry = nlohmann::json::array();
	// コンポーネントをシリアライズし、配列へ追加していく
	for (auto&& comp : m_components)
	{
		nlohmann::json compSerial = nlohmann::json::object();
		comp->Serialize(compSerial);

		// 配列へ追加
		compAry.push_back(compSerial);
	}

	outJson["Components"] = compAry;

	//-------------------------
	// 子を再帰実行
	//-------------------------
	// 全ての子を入れる配列
	nlohmann::json childAry = nlohmann::json::array();
	for (auto&& child : m_children)
	{
		nlohmann::json objSerial = nlohmann::json::object();
		child->Serialize(objSerial);

		// 配列へ追加
		childAry.push_back(objSerial);
	}

	outJson["Children"] = childAry;
}

void KdGameObject::Editor_ImGui()
{
	ImGui::Text(m_guid.ToString().c_str());

	// 有効フラグ
	ImGui::Checkbox("Enable", &m_enable);
	// 名前
	ImGui::InputText("m_name", &m_name);
	// Tag
	ImGui::InputText("m_tag", &m_tag);

	//-------------------------------
	// 全コンポーネント
	//-------------------------------
	for(size_t iComp = 0;iComp < m_components.size(); iComp++)
//	for (auto& it = m_components.begin(); it != m_components.end(); )
	{
		auto& comp = m_components[iComp];

		ImGui::PushID(comp.get());

		// コンポーネント名
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.5f, 0.2f, 0.2f, 1));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.8f, 0.2f, 0.2f, 1));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 0.2f, 0.2f, 1));

		bool bOpen = ImGui::CollapsingHeader(comp->GetClassName().c_str(), ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::PopStyleColor(3);

		// 説明
		//ImGuiShowHelp(comp->Editor_GetComment().c_str());

		/*
		// メニュー
		if (ImGui::BeginPopupContextItem("ComponentMenuPopup"))
		{
			if (ImGui::Selectable(u8"１つ上へ"))
			{
				GoPrevSiblingComponent(comp);
				bOpen = false;
			}

			if (ImGui::Selectable(u8"１つ下へ"))
			{
				GoNextSiblingComponent(comp);
				bOpen = false;
			}

			ImGui::Separator();
			{
				static json11::Json::object s_copyComponent;
				if (ImGui::Selectable(u8"コピー"))
				{
					comp->Serialize(s_copyComponent);
				}
				if (ImGui::Selectable(u8"貼り付け"))
				{
					if (comp->GetClassName() == s_copyComponent["ClassName"].string_value())
					{
						comp->Deserialize(s_copyComponent);
						bOpen = false;
					}
				}
			}
			ImGui::Separator();

			if (ImGui::Selectable(u8"削除"))
			{
				it = m_components.erase(it);
				bOpen = false;
			}

			ImGui::EndPopup();
		}
		*/

		// 内容
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
