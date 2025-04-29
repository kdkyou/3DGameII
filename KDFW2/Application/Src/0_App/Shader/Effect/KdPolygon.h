#pragma once


// 指定ポリゴン描画クラス

class KdPolygon
{
public:
	bool Initialize();
	void Release();

	// Effectに関する設定
	UINT AddVertex(const KdVector3& pos, const KdVector2& uv, const uint32_t& col = 0xFFFFFFFF);
	bool SetTexture( const std::shared_ptr<KdTexture>& tex );

	void Draw();

private:
	std::vector<KdVector3> m_positions;
	std::vector<KdVector2> m_uvs;
	std::vector<uint32_t> m_colors;

	std::shared_ptr<KdBuffer> m_VB_Pos = nullptr;
	std::shared_ptr<KdBuffer> m_VB_UV = nullptr;
	std::shared_ptr<KdBuffer> m_VB_Color = nullptr;

	std::shared_ptr<KdMaterial> m_material = nullptr;
};