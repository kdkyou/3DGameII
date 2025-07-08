#pragma once

#include "KdComponent.h"

#include "Math/KdMath.h"

class KdTransformComponent : public KdComponent
{
public:

	//=====================================
	// 
	// データ取得系
	// 
	//=====================================

	// ローカル座標取得
	const KdVector3&		GetLocalPosition() const { return m_localPosition; }
	// ローカル回転取得
	const KdQuaternion&		GetLocalRotation() const { return m_localRotation; }
	// ローカル拡大取得
	const KdVector3&		GetLocalScale() const { return m_localScale; }

	// ワールド座標取得
	KdVector3				GetPosition() const { return GetWorldMatrix().Translation(); }
	// ワールド回転取得
	KdQuaternion			GetRotation() const { return KdQuaternion::CreateFromRotationMatrix(GetWorldMatrix()); }

	// ローカル行列取得
	KdMatrix				GetLocalMatrix() const;
	// ワールド行列取得
	const KdMatrix&			GetWorldMatrix() const;

	//=====================================
	// 
	// データセット系
	// 
	//=====================================
	// ローカル座標セット
	void SetLocalPosition(const KdVector3& pos)
	{
		m_localPosition = pos;

		SetChanged();	// 変更をマーク
	}
	// ローカル回転セット
	void SetLocalRotation(const KdQuaternion& rotation)
	{
		m_localRotation = rotation;

		SetChanged();	// 変更をマーク
	}
	// ローカル拡大セット
	void SetLocalScale(const KdVector3& scale)
	{
		m_localScale = scale;

		SetChanged();	// 変更をマーク
	}

	// ワールド座標セット
	void SetPosition(const KdVector3& pos);
	// ワールド回転セット
	void SetRotation(const KdQuaternion& rotation);

	// ローカル行列をセット
	void SetLocalMatrix(const KdMatrix& mLocal);
	// ワールド行列をセット
	void SetWorldMatrix(const KdMatrix& mWorld);


	// 自分以下の子全てに、変更フラグをONにする
	void SetChanged();

	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSONデータから、クラスの内容を設定
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		// 継承元の関数も実行
		KdComponent::Deserialize(jsonObj);

		KdJsonUtility::GetArray(jsonObj, "Position", &m_localPosition.x, 3);
		KdJsonUtility::GetArray(jsonObj, "Rotation", &m_localRotation.x, 4);
		KdJsonUtility::GetArray(jsonObj, "Scale", &m_localScale.x, 3);
	}

	// このクラスの内容をJSONデータ化する
	virtual void Serialize(nlohmann::json& outJson) const
	{
		// 継承元の関数も実行
		KdComponent::Serialize(outJson);

		outJson["Position"] = KdJsonUtility::CreateArray(&m_localPosition.x, 3);
		outJson["Rotation"] = KdJsonUtility::CreateArray(&m_localRotation.x, 4);
		outJson["Scale"] = KdJsonUtility::CreateArray(&m_localScale.x, 3);
	}

	//===============================
	// Editor
	//===============================
	virtual void Editor_ImGui() override;

private:

	// ローカル座標
	KdVector3			m_localPosition;
	// ローカル回転
	KdQuaternion		m_localRotation;
	// ローカル拡大
	KdVector3			m_localScale = { 1,1,1 };


	// 計算済みのワールド行列(重複計算を出来るだけ避ける用)
	mutable KdMatrix		m_calcedWorldMatrix;
	// 変更時にONになる。ONのやつは行列の再計算が必要になる。
	mutable bool			m_matrixNeedUpdate = true;

};
