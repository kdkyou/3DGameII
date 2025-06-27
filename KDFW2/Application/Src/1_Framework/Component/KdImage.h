#pragma once

#include"KdComponent.h"

class KdPolygon;

class KdImage :public KdRendererComponent
{
public:
	virtual void Start()override;
	virtual void Update()override;

	virtual void PreDraw()override;
	virtual void Draw(bool opaque, KdShader::PassTags passTag)override;

	virtual void Editor_ImGui()override;

	virtual void Deserialize(const nlohmann::json& jsonObj) override;		// （読み込み）
	virtual void Serialize(nlohmann::json& outJson) const override;			// (保存)


	void CreateVertex();	// 読み込んだテクスチャ通りに頂点を作る
	
private:
	std::shared_ptr<KdPolygon> m_polygon = nullptr;
	// このコンポーネント上での表示サイズ
	int m_width = 100;
	int m_height = 100;

	// 読み込んだテクスチャ
	std::string m_imagePath = "";
	std::shared_ptr<KdTexture> m_texture = nullptr;


};