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


	//エフェクトを描画するポリゴン管理
	std::shared_ptr<KdPolygon> m_poly = nullptr;
	//エフェクトテクスチャ
	std::shared_ptr<KdTexture> m_texture = nullptr;

};