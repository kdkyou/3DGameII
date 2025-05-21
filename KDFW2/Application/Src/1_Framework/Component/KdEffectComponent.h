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

	virtual void Editor_ImGui()override;

private:

	//エフェクトを描画するポリゴン管理
	std::shared_ptr<KdPolygon> m_poly = nullptr;
	//エフェクトテクスチャ
	std::shared_ptr<KdTexture> m_texture = nullptr;

};