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
	// �e��ModelComponent�����邩
	m_targetModel = GetGameObject()->GetParent()->GetComponent< KdModelRendererComponent>();
	// ������Ȃ�����
	if (m_targetModel.expired() == true) { return false; }

	auto modelComp = m_targetModel.lock();

	// ���f���R���|�[�l���g�̒�����Y���̃m�[�h��T��

	// ���O�������Ă��邩
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

	// Model��Transform���擾
	auto modelComp = m_targetModel.lock();
	auto targetTrans = modelComp->GetNodeTransformFromIndex(m_targetIndex);

	// ���鑤�Ə㏑������鑤
	auto src = GetGameObject()->GetTransform()->GetLocalMatrix();
	auto dst = targetTrans->GetWorldMatrix();

	
	//���]
	if (m_reverse == true)
	{ 
		auto tmp = src;
		src = dst;
		dst = tmp;
	}

	// <--------------�e��Œ�-------------------------
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
	


	// Transform�𑗂荞��
	if (m_reverse == false)
	{
		// GameObject(�e)��Nonde(�q)
		targetTrans->SetWorldMatrix(src);
	}
	else
	{
		// Node(�e)��GameObject(�q)
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

	//�e��������m�[�h�̖��O

	if (ImGui::InputText(u8"Node��", &m_nodeName))
	{
		Connect(); // ���͂��ꂽ���e�ŒT��
	}

	// ������Ȃ��������̌x��
	auto modelComp = m_targetModel.lock();
	if(modelComp == nullptr) {
		ImGui::Text(u8"��ModelComponent��������܂���");
	}
	if (m_targetIndex == -1) {
		ImGui::Text(u8"��Node��������܂���");
	}

	ImGui::Checkbox(u8"����GameObject��Node��Transform��K�p", &m_reverse);

	ImGui::Checkbox(u8"���W�Œ�",&m_lockPos);
	if (m_lockPos == true)
	{
		ImGui::DragFloat3(u8"���W", &m_localPos.x, 0.0f, 100.0f);
	}
	ImGui::Checkbox(u8"��]�Œ�", &m_lockRotate);
	if (m_lockRotate == true)
	{
		ImGui::DragFloat3(u8"��]", &m_localRotate.x, 0.0f, 100.0f);
	}
	ImGui::Checkbox(u8"�g�k�Œ�", &m_lockScale);
	if (m_lockScale == true)
	{
		ImGui::DragFloat3(u8"�g�k", &m_localScale.x, 0.0f, 100.0f);
	}
	
}



