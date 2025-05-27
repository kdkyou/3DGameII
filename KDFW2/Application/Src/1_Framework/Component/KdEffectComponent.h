#pragma once

#include"KdComponent.h"

class KdPolygon;	//前方宣言

//エフェクト管理コンポーネント
class KdEffectComponent : public KdRendererComponent
{
public:
	
	virtual void Start() override;
	virtual void Update()override;
	virtual void LateUpdate()override;
	virtual void PreDraw()override;
	virtual void Draw(bool opaque,KdShader::PassTags passTag)override;

	//エディタ画面
	virtual void Editor_ImGui()override;

	//保存読込
	virtual void Deserialize(const nlohmann::json& jsonObj)override;
	virtual void Serialize(nlohmann::json& outJson) const override;

private:

	//ﾋﾞﾙﾎﾞｰﾄﾞ化した後の追加のZ回転
	float m_zRotation = 0.0f;

	KdVector2 m_scale = { 1.0f,1.0f };

	KdVector3 m_color = { 1.0f,1.0f,1.0f };

	//アニメーション動作	課題
	// m_lifetimeの時間をかけて
	// m_startScaleeからm_endScale まで拡縮を変更(線形補間)
	// アニメーションは保存できるようにする
	
	//エフェクトのアニメーション時間
	float m_lifetime = 3.0f;	
	//開始時の拡縮率
	KdVector2 m_startScale = {1,1};
	//終了時の拡縮率
	KdVector2 m_endScale = {3,5};

	// Startと同時に拡縮をするかどうか
	bool m_playWithStart = false;	


	// アニメーションが開始したか
	bool m_isStarted = false;
	//開始してからの経過時間
	float m_duration = 0.0f;

	KdEase			m_ease;

	void StartAnim();

	//エフェクトを描画するポリゴン管理
	std::shared_ptr<KdPolygon> m_poly = nullptr;
	//エフェクトテクスチャ
	std::shared_ptr<KdTexture> m_texture = nullptr;

	//使用するテクスチャのpath
	std::string m_textureFilePath = "";

};