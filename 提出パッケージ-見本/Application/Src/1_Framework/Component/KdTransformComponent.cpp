#include "KdTransformComponent.h"

#include "KdGameObject.h"

#include "KdFramework.h"

#include "CoreSystem.h"

#include "Editor/KdEditorData.h"

SetClassAssembly(KdTransformComponent, "Component");

// ローカル行列取得
KdMatrix KdTransformComponent::GetLocalMatrix() const
{
	// 座標、回転、拡大から行列を作成
	return	DirectX::XMMatrixScalingFromVector(m_localScale) *
			DirectX::XMMatrixRotationQuaternion(m_localRotation) *
			DirectX::XMMatrixTranslationFromVector(m_localPosition);
}

// ワールド行列取得
const KdMatrix& KdTransformComponent::GetWorldMatrix() const
{
	// 行列更新必要フラグがONなら、計算を行う
	if (m_matrixNeedUpdate)
	{
		// 親と合成
		auto parent = GetGameObject()->GetParent();
		if (parent != nullptr)
		{
			m_calcedWorldMatrix = GetLocalMatrix() * parent->GetTransform()->GetWorldMatrix();
		}
		else
		{
			m_calcedWorldMatrix = GetLocalMatrix();
		}

		m_matrixNeedUpdate = false;
	}

	// 計算済みの行列を返す
	return m_calcedWorldMatrix;
}

// ワールド座標セット
void KdTransformComponent::SetPosition(const KdVector3& pos)
{
	KdMatrix m = KdMatrix::CreateTranslation(pos);

	// ローカルに変換
	auto parent = GetGameObject()->GetParent();
	if (parent != nullptr)
	{
		m *= parent->GetTransform()->GetWorldMatrix().Invert();
	}

	SetLocalPosition(m.Translation());
}

// ワールド回転セット
void KdTransformComponent::SetRotation(const KdQuaternion& rotation)
{
	KdMatrix m = m.CreateFromQuaternion(rotation);

	// ローカルに変換
	auto parent = GetGameObject()->GetParent();
	if (parent != nullptr)
	{
		m *= parent->GetTransform()->GetWorldMatrix().Invert();
	}

	SetLocalRotation(KdQuaternion::CreateFromRotationMatrix(m));
}

// ローカル行列をセット
void KdTransformComponent::SetLocalMatrix(const KdMatrix& mLocal)
{
	// 拡大、回転、座標に分割
	DirectX::XMVECTOR s, r, t;
	DirectX::XMMatrixDecompose(&s, &r, &t, mLocal);

	m_localScale = s;
	m_localRotation = r;
	m_localPosition = t;

	// 変更をマーク
	SetChanged();

}

// ワールド行列をセット
void KdTransformComponent::SetWorldMatrix(const KdMatrix& mWorld)
{
	m_calcedWorldMatrix = mWorld;

	// ローカルに変換
	auto parent = GetGameObject()->GetParent();
	if (parent != nullptr)
	{
		KdMatrix mLocal = m_calcedWorldMatrix * parent->GetTransform()->GetWorldMatrix().Invert();
		SetLocalMatrix(mLocal);
	}
	else
	{
		SetLocalMatrix(m_calcedWorldMatrix);
	}
}

void KdTransformComponent::Editor_ImGui()
{
	KdComponent::Editor_ImGui();

	KdMatrix mWorld = GetWorldMatrix();

	//-------------------------------
	// 数値入力
	//-------------------------------
	KdVector3 pos;
	KdVector3 angles;
	KdVector3 scale;
	KdMatrix mLocal = GetLocalMatrix();
	
	ImGuizmo::DecomposeMatrixToComponents(&mLocal._11, &pos.x, &angles.x, &scale.x);

	// 座標
	if (ImGui::DragFloat3(u8"座標", &pos.x, 0.01f))
	{
		SetLocalPosition(pos);
	}

	// 回転(オイラー角)
	if (ImGui::DragFloat3(u8"回転", &angles.x, 1))
	{
		// 回転角度からクォータニオンへ
		KdMatrix m;
		ImGuizmo::RecomposeMatrixFromComponents(&KdVector3::Zero.x, &angles.x, &KdVector3::One.x, &m._11);
		SetLocalRotation(KdQuaternion::CreateFromRotationMatrix(m));
	}

	// 拡大
	if (ImGui::DragFloat3(u8"拡大", &scale.x, 0.01f))
	{
		SetLocalScale(scale);
	}

	//-------------------------------
	// マニピュレータ
	//-------------------------------
	{
		KdMatrix mView = KdEditorData::GetInstance().m_editorCamera->GetCameraMatrix().Invert();
		KdMatrix mProj = KdEditorData::GetInstance().m_editorCamera->GetProjMatrix();



		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::Manipulate(&mView._11, &mProj._11, KdEditorData::GetInstance().GizmoOperation, KdEditorData::GetInstance().GizmoMode, &mWorld._11);


		if (ImGuizmo::IsUsing())
		{
			SetWorldMatrix(mWorld);
		}
	}
}

void KdTransformComponent::SetChanged()
{
	m_matrixNeedUpdate = true;

	// 子もセット
	for (auto&& gameObj : GetGameObject()->GetChildren())
	{
		// すでに変更フラグがONの子は、スキップする(こいつ以下もONになっているため)
		if (gameObj->GetTransform()->m_matrixNeedUpdate)return;

		gameObj->GetTransform()->SetChanged();
	}
}
