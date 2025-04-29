#pragma once

#include "../KdDirectX.h"

#include "../KdGraphicsStates.h"

#include "BlitShader/KdBlitShader.h"
#include "SkinningShader/KdSkinningShader.h"

#include "KdShader.h"


//==========================================================
//
// シェーダ関係をまとめたクラス
//
//==========================================================
class KdShaderManager
{
public:

	//==============================================================
	//
	// 初期化・解放
	//
	//==============================================================

	// 初期化
	void Init();

	// 解放
	void Release();

	//==========================
	//
	// シェーダ
	//
	//==========================
	// 簡易2D描画シェーダー
	KdBlitShader			m_blitShader;
	KdSkinningShader		m_skinningShader;


	// 
	std::shared_ptr<KdShader> GetShader(const std::string& name)
	{
		auto it = m_shaderMap.find(name);
		if (it == m_shaderMap.end())return nullptr;

		return it->second;
	}
	void AddShader(const std::string& name, std::shared_ptr<KdShader> shader)
	{
		shader->SetName(name);
		m_shaderMap[name] = shader;
	}

	const std::unordered_map<std::string, std::shared_ptr<KdShader>>& GetShaderMap() const
	{
		return m_shaderMap;
	}

	//==========================
	// 共通テクスチャ
	//==========================

	// 予約テクスチャスロット番号
	enum TextureSlots
	{
		Color = 100,
		Normal = 101,
		Depth = 102,
		DirLightShadowMap = 103,
		IBL = 110,
	};

	//================================
	// 定数バッファ スロット番号定数
	//================================
	enum ConstantBufferSlots
	{
		PerMaterial = 0,		// マテリアル単位
		PerDraw = 1,			// 描画単位
		PerCamera = 7,			// カメラ単位
		Light = 8,				// ライト
		System = 13,			// システム
	};

	//================================
	// 描画関係 定数バッファデータ
	//================================

	struct CBPerDraw
	{
		KdMatrix mW;						// ワールド変換行列
	};

	std::shared_ptr<KdConstantBufferType<CBPerDraw>> m_cbPerDraw;


	//================================
	// カメラ関係 定数バッファデータ
	//================================

	// カメラ定数バッファ用構造体
	struct CBPerCamera
	{
		KdMatrix				mV;			// ビュー行列
		KdMatrix				mInvV;		// ビュー行列(逆行列)
		KdMatrix				mP;			// 射影行列
		KdMatrix				mInvP;		// 射影行列(逆行列)
		KdMatrix				mVP;		// ビュー*射影行列
		KdMatrix				mInvVP;		// ビュー*射影行列(逆行列)
		KdVector3				CamPos;		// カメラ座標(ワールド)

		void SetDatas(const KdMatrix& cameraMatrix, const KdMatrix& projMatrix)
		{
			mInvV = cameraMatrix;
			mV = cameraMatrix.Invert();
			CamPos = cameraMatrix.Translation();

			mP = projMatrix;
			mInvP = mP.Invert();

			mVP = mV * mP;
			mInvVP = mVP.Invert();
		}

		void SetCamera(const KdMatrix& cameraMatrix)
		{
			mInvV = cameraMatrix;
			mV = cameraMatrix.Invert();
			CamPos = cameraMatrix.Translation();

			mVP = mV * mP;
			mInvVP = mVP.Invert();
		}

		void SetPerspectiveFov(float fov, float aspect, float nearZ, float farZ)
		{
			mP = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
			mInvP = mP.Invert();

			mVP = mV * mP;
			mInvVP = mVP.Invert();
		}
	};

	std::shared_ptr<KdConstantBufferType<CBPerCamera>>	m_cbPerCamera;

	//==========================
	//
	// ライト関係
	//
	//==========================

	// ライト定数バッファ用構造体
	struct CBLight
	{
//		int PL_Cnt = 0;
		int SL_Count = 0;
		float tmp[3];

		//--------------------------
		// 環境光
		//--------------------------
		KdVector3					AmbientLight = {0.2f, 0.2f, 0.2f};
		float					tmp2;

		//--------------------------
		// 平行光
		//--------------------------
		KdVector3	DL_Dir = { 0,-1,0 };    // 平行光の方向
//			float	tmp3;
		float DL_DirLightShadowBias;	// 平行光のシャドウマップ用バイアス
		KdVector3	DL_Color = { 3,3,3 };	// 平行光の色
//			float	tmp4;
		float DL_ShadowPower = 0;
		KdMatrix DL_mLightVP;				// 平行光のビュー*射影行列

		//--------------------------
		// スポット光
		//--------------------------
		struct SpotLight
		{
			KdVector3		Dir;			// 光の方向
			float		Range;			// 範囲
			KdVector3		Color;			// 色
			float		InnerCorn;		// 内側の角度
			KdVector3		Pos;			// 座標
			float		OuterCorn;		// 外側の角度

			float		EnableShadow;
			float		ShadowBias = 0.01f;
			float		tmp[2];
			KdMatrix		mLightVP;
		};
		std::array<SpotLight, 10>	SL;

		/*
		//--------------------------
		// 点光
		//--------------------------
		struct PointLight
		{
			Vector3	Color;			// 色
			float	Radius;			// 半径
			Vector3	Pos;			// 座標
			float tmp;
		};
		std::array<PointLight, 100>	PL;
		*/

		//--------------------------
		// 距離フォグ
		//--------------------------
		KdVector3	DistanceFogColor;
		float		DistanceFogDensity = 0;

		void Reset()
		{
			DL_ShadowPower = 0;
			DL_DirLightShadowBias = 0.002f;
			DL_Color = { 0,0,0 };

			SL_Count = 0;
		}
	};

	// ライト定数バッファ
	std::shared_ptr<KdConstantBufferType<CBLight>>	m_cbLight;

	//==========================
	// 
	// システム
	// 
	//==========================
	struct CBSystem
	{
		float Time;
	};

	// システム定数バッファ
	std::shared_ptr<KdConstantBufferType<CBSystem>>	m_cbSystem;

	//==========================
	//
	// パイプラインステート関係
	//
	//==========================

	// サンプラ
	std::shared_ptr<KdSamplerState> m_ss_Anisotropic_Wrap = nullptr;
	std::shared_ptr<KdSamplerState> m_ss_Anisotropic_Clamp = nullptr;
	std::shared_ptr<KdSamplerState> m_ss_Linear_Wrap = nullptr;
	std::shared_ptr<KdSamplerState> m_ss_Linear_Clamp = nullptr;
	std::shared_ptr<KdSamplerState> m_ss_Point_Wrap = nullptr;
	std::shared_ptr<KdSamplerState> m_ss_Point_Clamp = nullptr;
	std::shared_ptr<KdSamplerState> m_ss_Linear_Comparison_Clamp = nullptr;

	void SetCommonSamplers();

	// デプスステンシル
	std::shared_ptr<KdDepthStencilState> m_ds_ZEnable_ZWriteEnable = nullptr;
	std::shared_ptr<KdDepthStencilState> m_ds_ZEnable_ZWriteDisable = nullptr;
	std::shared_ptr<KdDepthStencilState> m_ds_ZDisable_ZWriteDisable = nullptr;

	// ラスタライズステート
	std::shared_ptr<KdRasterizerState> m_rs_CullBack = nullptr;
	std::shared_ptr<KdRasterizerState> m_rs_CullNone = nullptr;
	std::shared_ptr<KdRasterizerState> m_rs_Wireframe = nullptr;

	// ブレンドステート
	std::shared_ptr<KdBlendState> m_bs_Copy = nullptr;
	std::shared_ptr<KdBlendState> m_bs_Alpha = nullptr;
	std::shared_ptr<KdBlendState> m_bs_Add = nullptr;
	std::shared_ptr<KdBlendState> m_bs_Multiply = nullptr;

	//==========================
	//
	// その他
	//
	//==========================

private:

	// シェーダーマップ
	std::unordered_map<std::string, std::shared_ptr<KdShader>> m_shaderMap;

//-------------------------------
// シングルトン
//-------------------------------
private:

	KdShaderManager() {}

public:
	static KdShaderManager &GetInstance() {
		static KdShaderManager instance;
		return instance;
	}
};

#define SHADER KdShaderManager::GetInstance()
