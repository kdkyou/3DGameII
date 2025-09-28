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
		{

		auto mP = targetTrans->GetWorldMatrix();
		auto mC = trans->GetWorldMatrix();

		// 初回
		if (m_offset == nullptr) {
			m_offset =std::make_shared<KdMatrix>();
			*m_offset = mC * mP.Invert();
		}

		trans->SetWorldMatrix(*m_offset * mP);
		}

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

	// 追尾方法を選択
	bool selItem[Type::Num] = {};
	selItem[m_chaseType]= true;
	if (ImGui::BeginListBox(u8"追従方法"))
	{
		if(ImGui::Selectable(u8"完全追従", selItem[Totally])) { m_chaseType = Totally; }
		if(ImGui::Selectable(u8"位置だけ", selItem[Position])) { m_chaseType = Position; }
		if(ImGui::Selectable(u8"回転だけ", selItem[Rotation])) { m_chaseType = Rotation; }
		if(ImGui::Selectable(u8"親子構造", selItem[Parent])) { m_chaseType = Parent; }
		if(ImGui::Selectable(u8"向くだけ", selItem[LookAt])) { m_chaseType = LookAt; }
		ImGui::EndListBox();
	}

	if (m_chaseType != Parent && m_offset != nullptr) 
	{
		m_offset = nullptr;
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
