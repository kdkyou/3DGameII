#pragma once

#include <d3d11.h>

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include <SimpleMath.h>

#include "2_CoreSystem/nlohmann/KdJsonUtility.h"

namespace Math = DirectX::SimpleMath;

// 円周率
static const float KdPI = 3.14159265358979323846f;
// ラジアン -> 度 変換用
static const float KdRad2Deg = (180.f / KdPI);
// 度 -> ラジアン 変換用
static const float KdDeg2Rad = (KdPI / 180.f);

//===============================================================
// 2Dベクトル(SimpleMathをそのまま使用)
//===============================================================
using KdVector2 = DirectX::SimpleMath::Vector2;

//===============================================================
// 3Dベクトル(SimpleMathをそのまま使用)
//===============================================================
using KdVector3 = DirectX::SimpleMath::Vector3;

//===============================================================
// 4Dベクトル(SimpleMathをそのまま使用)
//===============================================================
using KdVector4 = DirectX::SimpleMath::Vector4;

//===============================================================
// クォータニオン(SimpleMathをそのまま使用)
//===============================================================
using KdQuaternion = DirectX::SimpleMath::Quaternion;

//===============================================================
// 4x4行列(SimpleMathをそのまま使用)
//===============================================================
using KdMatrix = DirectX::SimpleMath::Matrix;

//===============================================================
// 色(SimpleMathをそのまま使用)
//===============================================================
using KdColor = DirectX::SimpleMath::Color;

//===============================================================
//
// トランスフォーム
// 
// ・座標、回転、拡大データを持ち、行列への相互変換ができるもの。
// 
//===============================================================
class KdTransform
{
public:

	//=====================================
	// 
	// データ取得系
	// 
	//=====================================
	// (取得)ローカル座標
	const KdVector3&		GetLocalPosition() const { return m_localPosition; }
	// (取得)ローカル回転
	const KdQuaternion&		GetLocalRotation() const { return m_localRotation; }
	// (取得)ローカル拡大
	const KdVector3&		GetLocalScale() const { return m_localScale; }

	// (取得)ワールド座標
	KdVector3				GetPosition() const { return GetWorldMatrix().Translation(); }
	// (取得)ワールド回転
	KdQuaternion			GetRotation() const { return KdQuaternion::CreateFromRotationMatrix(GetWorldMatrix()); }

	// (取得)ローカル行列
	KdMatrix GetLocalMatrix() const
	{
		// 座標、回転、拡大から行列を作成
		return	DirectX::XMMatrixScalingFromVector(m_localScale) *
				DirectX::XMMatrixRotationQuaternion(m_localRotation) *
				DirectX::XMMatrixTranslationFromVector(m_localPosition);
	}

	// (取得)ワールド行列(階層構造有りの行列)
	const KdMatrix& GetWorldMatrix() const
	{
		// 行列更新必要フラグがONの時のみ、計算を行う
		if (m_matrixNeedUpdate)
		{
			// 親の行列と合成
			if (m_parent != nullptr)
			{
				// 自分のローカル行列 * 親のワールド行列
				m_calcedWorldMatrix = GetLocalMatrix() * m_parent->GetWorldMatrix();
			}
			// 親無し
			else
			{
				m_calcedWorldMatrix = GetLocalMatrix();
			}

			// 行列更新必要フラグをOFF
			m_matrixNeedUpdate = false;
		}

		// 計算済みの行列を返す
		return m_calcedWorldMatrix;
	}

	//=====================================
	// 
	// データセット系
	// 
	//=====================================
	// (代入)ローカル座標
	void SetLocalPosition(const KdVector3& pos)
	{
		m_localPosition = pos;

		// ワールド行列を更新する必要があるので、フラグをONにする
		SetChanged();
	}
	// (代入)ローカル回転
	void SetLocalRotation(const KdQuaternion& rotation)
	{
		m_localRotation = rotation;

		// ワールド行列を更新する必要があるので、フラグをONにする
		SetChanged();
	}
	// (代入)ローカル拡大
	void SetLocalScale(const KdVector3& scale)
	{
		m_localScale = scale;

		// ワールド行列を更新する必要があるので、フラグをONにする
		SetChanged();
	}

	// (代入)ワールド座標
	void SetPosition(const KdVector3& pos)
	{
		// ローカルに変換
		KdMatrix m = KdMatrix::CreateTranslation(pos);
		if (m_parent != nullptr)
		{
			m *= m_parent->GetWorldMatrix().Invert();
		}

		SetLocalPosition(m.Translation());
	}
	// (代入)ワールド回転
	void SetRotation(const KdQuaternion& rotation)
	{
		// ローカルに変換
		KdMatrix m = m.CreateFromQuaternion(rotation);
		if (m_parent != nullptr)
		{
			m *= m_parent->GetWorldMatrix().Invert();
		}

		SetLocalRotation(KdQuaternion::CreateFromRotationMatrix(m));
	}


	// (代入)ローカル行列
	void SetLocalMatrix(const KdMatrix& mLocal)
	{
		// 拡大、回転、座標に分割
		DirectX::XMVECTOR s, r, t;
		DirectX::XMMatrixDecompose(&s, &r, &t, mLocal);

		m_localScale = s;
		m_localRotation = r;
		m_localPosition = t;

		// ワールド行列を更新する必要があるので、フラグをONにする
		SetChanged();
	}

	// (代入)ワールド行列
	void SetWorldMatrix(const KdMatrix& mWorld)
	{
		m_calcedWorldMatrix = mWorld;

		if (m_parent != nullptr)
		{
			// ローカル行列に変換
			KdMatrix mLocal = m_calcedWorldMatrix * m_parent->GetWorldMatrix().Invert();

			// ローカル行列セット
			SetLocalMatrix(mLocal);
		}
		else
		{
			// ローカル行列セット
			SetLocalMatrix(m_calcedWorldMatrix);
		}
	}

	//=====================================
	// 
	// シリアライズ/デシリアライズ
	// 
	//=====================================

	// JSON化
	nlohmann::json ToJson() const
	{
		nlohmann::json js = nlohmann::json::object();
		js["Position"] = KdJsonUtility::CreateArray(&m_localPosition.x, 3);
		js["Rotation"] = KdJsonUtility::CreateArray(&m_localRotation.x, 4);
		js["Scale"] = KdJsonUtility::CreateArray(&m_localScale.x, 3);
		return js;
	}
	// JSONからデータセット
	void FromJson(const nlohmann::json &jsonObj)
	{
		KdJsonUtility::GetArray(jsonObj, "Position", &m_localPosition.x, 3);
		KdJsonUtility::GetArray(jsonObj, "Rotation", &m_localRotation.x, 4);
		KdJsonUtility::GetArray(jsonObj, "Scale",	&m_localScale.x, 3);
	}

private:

	// 自分以下の子全てに、ワールド行列更新フラグをONにする
	void SetChanged()
	{
		// ワールド行列の変更が必要フラグをONにする
		m_matrixNeedUpdate = true;

		// 子もセットする(再帰)
		for (auto&& child : m_children)
		{
			// すでに変更フラグがONの子は、スキップする(こいつ以下もONになっているため)
			if (child->m_matrixNeedUpdate)return;

			child->SetChanged();
		}
	}

private:

	// ローカル座標
	KdVector3			m_localPosition;
	// ローカル回転
	KdQuaternion		m_localRotation;
	// ローカル拡大
	KdVector3			m_localScale = { 1,1,1 };

	//---------------
	// 階層構造時
	//---------------
	// 計算済みワールド行列
	mutable KdMatrix	m_calcedWorldMatrix;

	// ワールド行列変更必要フラグ
	// これがONの時は、ワールド行列を再計算する必要がある
	mutable bool		m_matrixNeedUpdate = true;

public:
	// 階層構造にする場合、親への参照
	KdTransform*				m_parent = nullptr;
	// 階層構造にする場合、子への参照配列
	std::vector<KdTransform*>	m_children;
};
