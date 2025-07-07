#include "KdTransformConstraint.h"
#include"Editor/KdEditorData.h"

SetClassAssembly(KdTransformConstraint, "Component");

void KdTransformConstraint::Start()
{
	// すでにGuidが設定されていた
	if (m_targetGuid != "")
	{
		// GUIDが振り直されたかどうかを調べる
		m_targetGuid = KdGuid::GetRePlacedGuid(m_targetGuid);

		// 新たに作成されたであろうGameObjectを探す
		auto obj = KdFramework::GetInstance().GetScene()
			->FindObjectWithGuid(m_targetGuid);

		if (obj != nullptr)
		{
			m_target = obj;
		}
	}
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
		trans->SetRotation(targetTrans->GetRotation());
		break;
	case Parent:
		// 相対座標を取得するために必要なのは
		// 自身のLocal座標とtargetのworld座標,
		// しかし、そのまま計算をするとどっか行く
		// ワールド行列にターゲットの行列を取得
		trans->SetWorldMatrix((trans->GetWorldMatrix()*targetTrans->GetWorldMatrix()));

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
	else if (m_targetGuid != "")
	{
		// IDは登録されているのに、対象が居ない
		targetName = "missing";
	}
	ImGui::LabelText(u8"ターゲット名", targetName.c_str());

	static Type sel = None;
	if (m_target.expired() != true)
	{
		if (ImGui::BeginListBox(u8"追跡方法"))
		{
			if (ImGui::Selectable("Totally")) { sel = Totally; }
			if (ImGui::Selectable("Position")) { sel = Position; }
			if (ImGui::Selectable("Rotation")) { sel = Rotation; }
			if (ImGui::Selectable("Parent")) { sel = Parent; }
			if (ImGui::Selectable("LookAt")) { sel = LookAt; }

			ImGui::EndListBox();
		}
		if (sel != None && ImGui::Button(u8"決定"))
		{
			m_chaseType = sel;
		}
	}

}

void KdTransformConstraint::Serialize(nlohmann::json& outJson)const
{
	KdComponent::Serialize(outJson);

	outJson["TargetGuid"] = m_targetGuid;

}

void KdTransformConstraint::Deserialize(const nlohmann::json& jsonObj)
{
	KdComponent::Deserialize(jsonObj);

	KdJsonUtility::GetValue(jsonObj, "TargetGuid", &m_targetGuid);

}

void KdTransformConstraint::Set(std::shared_ptr<KdGameObject> target, Type chaseType)
{
}
