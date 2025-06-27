#include "KdTransformConstraint.h"
#include"Editor/KdEditorData.h"

SetClassAssembly(KdTransformConstraint, "Component");

void KdTransformConstraint::Start()
{

}
void KdTransformConstraint::Update()
{
	if (m_enable == false) { return; }

	// 追従ターゲット取得
	if (m_target.expired() == true) { return; }
	auto target = m_target.lock();

	// 追従ターゲットのTransform
	auto targetTrans = target->GetTransform();
	// 自分のTransform
	auto trans = GetGameObject()->GetTransform();

	// 指定した追いかけ方で追従
	switch (m_chaseType)
	{
	case Totally:
		trans->SetWorldMatrix(targetTrans->GetWorldMatrix());
		break;
	case Position:
		trans->SetPosition(targetTrans->GetPosition());
		break;
	case Rotation:

		break;
	case Parent:
		break;
	case LookAt:
		auto vPos = trans->GetPosition();
		auto vtPos = targetTrans->GetPosition();

		// 振り向きが不可
		if ((vPos - vtPos).Length() <= 0) { return; }
		KdMatrix mRes = DirectX::XMMatrixLookAtLH(
			vPos, vtPos, KdVector3(0, 1, 0)
		);
		// 自分から相手を向いた行列に変換して設定
		trans->SetWorldMatrix(mRes.Invert());

		break;
	}

}
void KdTransformConstraint::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	// TargetになるGameObjectのGUIDを設定する
	if (KdEditorData::GetInstance().Clipboard != "" &&
		ImGui::Button(u8"コピーしたGameObjectをターゲット")) 
	{
		// Hierarchyの中に外套のオブジェクトがいるか
		auto obj = KdFramework::GetInstance().GetScene()->
			FindObjectWithGuid(KdEditorData::GetInstance().Clipboard);
		if (obj != nullptr)
		{
			m_target = obj;
			m_targetGuid = KdEditorData::GetInstance().Clipboard;
		}
	}
	// ターゲット名
	std::string targetName = "None";
	// ターゲットがいるs
	if (m_target.expired() == false)
	{
		auto target = m_target.lock();
		targetName = target->GetName();
	}
	else if(m_targetGuid != "")
	{
		// IDは登録されているのに、対象が居ない
		targetName = "missing";
	}
	ImGui::LabelText(u8"ターゲット名", targetName.c_str());

}

void KdTransformConstraint::Serialize(nlohmann::json& outJson)const
{
	KdComponent::Serialize(outJson);
}

void KdTransformConstraint::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

}

void KdTransformConstraint::Set(std::shared_ptr<KdGameObject> target, Type chaseType)
{
}
