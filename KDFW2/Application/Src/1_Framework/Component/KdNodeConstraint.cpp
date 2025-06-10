#include "KdNodeConstraint.h"

#include"KdFramework.h"
#include"KdModelRendererComponent.h"

SetClassAssembly(KdNodeConstraint, "Component");

void KdNodeConstraint::Start()
{
	Connect();
}

bool KdNodeConstraint::Connect()
{
	// 親にModelComponentがあるか
	m_targetModel = GetGameObject()->GetParent()->GetComponent< KdModelRendererComponent>();
	// 見つからなかった
	if (m_targetModel.expired() == true) { return false; }

	auto modelComp = m_targetModel.lock();

	// モデルコンポーネントの中から該当のノードを探す

	// 名前が入っているか
	if (m_nodeName != "") 
	{
		m_targetIndex = modelComp->GetNodeIndexFromName(m_nodeName);

		if (m_targetIndex == -1){ return false;}
	}

	return true;
}

void KdNodeConstraint::Update()
{
	if (m_targetModel.expired() == true) { return; }
	if (m_enable == false) { return; }
	if (m_targetIndex == -1) { return; }

	// ModelのTransformを取得
	auto modelComp = m_targetModel.lock();
	auto targetTrans = modelComp->GetNodeTransformFromIndex(m_targetIndex);

	// 送る側と上書きされる側
	auto src = GetGameObject()->GetTransform()->GetLocalMatrix();
	auto dst = targetTrans->GetWorldMatrix();

	
	//反転
	if (m_reverse == true)
	{ 
		auto tmp = src;
		src = dst;
		dst = tmp;
	}

	// <--------------各種固定-------------------------
	KdMatrix localMat = KdMatrix::Identity;
	KdMatrix transMat = KdMatrix::Identity;
	KdMatrix rotateMat = KdMatrix::Identity;
	KdMatrix scaleMat = KdMatrix::Identity;
	if (m_lockPos == true)
	{
		transMat = KdMatrix::CreateTranslation(m_localPos);
	}

	if (m_lockRotate == true)
	{
		rotateMat = KdMatrix::CreateFromYawPitchRoll(m_localRotate);
	}
	
	if (m_lockScale == true)
	{
		scaleMat = KdMatrix::CreateScale(m_localScale);
	}

	localMat = scaleMat * rotateMat * transMat;
	src = localMat * src;
	// --------------------------------------->
	


	// Transformを送り込む
	if (m_reverse == false)
	{
		// GameObject(親)→Nonde(子)
		targetTrans->SetWorldMatrix(src);
	}
	else
	{
		// Node(親)→GameObject(子)
		GetGameObject()->GetTransform()->SetLocalMatrix(src);
	}
		

}

void KdNodeConstraint::Serialize(nlohmann::json& outJson) const
{
	KdComponent::Serialize(outJson);

	outJson["NodeName"] = m_nodeName;
	outJson["Reverse"] = m_reverse;
	outJson["LockPos"] = m_lockPos;
	outJson["LocalPos"] = KdJsonUtility::CreateArray(&m_localPos.x,3);
	outJson["LockRotate"] = m_lockRotate;
	outJson["LocalRotate"] =KdJsonUtility::CreateArray(&m_localRotate.x,3);
	
}

void KdNodeConstraint::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj,"NodeName",&m_nodeName);
	KdJsonUtility::GetValue(jsonObj, "Reverse", &m_reverse);
	KdJsonUtility::GetValue(jsonObj, "LockPos", &m_lockPos);
	KdJsonUtility::GetArray(jsonObj, "LocalPos", &m_localPos.x,3);

}

void KdNodeConstraint::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	//影響させるノードの名前

	if (ImGui::InputText(u8"Node名", &m_nodeName))
	{
		Connect(); // 入力された内容で探す
	}

	// 見つからなかった時の警告
	auto modelComp = m_targetModel.lock();
	if(modelComp == nullptr) {
		ImGui::Text(u8"※ModelComponentが見つかりません");
	}
	if (m_targetIndex == -1) {
		ImGui::Text(u8"※Nodeが見つかりません");
	}

	ImGui::Checkbox(u8"このGameObjectにNodeのTransformを適用", &m_reverse);

	ImGui::Checkbox(u8"座標固定",&m_lockPos);
	if (m_lockPos == true)
	{
		ImGui::DragFloat3(u8"座標", &m_localPos.x, 0.0f, 100.0f);
	}
	ImGui::Checkbox(u8"回転固定", &m_lockRotate);
	if (m_lockRotate == true)
	{
		ImGui::DragFloat3(u8"回転", &m_localRotate.x, 0.0f, 100.0f);
	}
	ImGui::Checkbox(u8"拡縮固定", &m_lockScale);
	if (m_lockScale == true)
	{
		ImGui::DragFloat3(u8"拡縮", &m_localScale.x, 0.0f, 100.0f);
	}
	
}



