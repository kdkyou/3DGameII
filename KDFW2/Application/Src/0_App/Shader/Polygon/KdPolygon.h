#pragma once

//指定した頂点で面を作成するクラス
class KdPolygon 
{
public:

	// ポリゴンに張り付けるテクスチャ 5/19追加
	std::shared_ptr<KdTexture> m_tex = nullptr;
	
	bool Initialize();
	void Release();
	void Draw();

	//描画する頂点の座標とUVとColorを追加
	UINT AddVertex(const KdVector3& pos,const KdVector2& uv, const uint32_t& col= 0xFFFFFFFF);

	//バッファの作成
	void CreateBuffers();

	//作成したバッファをシェーダーにセット
	void SetBuffers();

	//現在の頂点数を返す
//	UINT GetLength() { m_positions.size(); }

	//外部から画像を受け取る
	bool SetTexture(const std::string& path);

private:

	//描画する頂点の座標
	std::vector<KdVector3> m_positions;
	//頂点のUV座標	5/20
	std::vector<KdVector2> m_uvs;
	//頂点の色		5/20
	std::vector<uint32_t> m_colors;


	// バッファ
	std::shared_ptr<KdBuffer> m_VB_Pos = nullptr;
	//	5/20
	std::shared_ptr<KdBuffer> m_VB_UV = nullptr;
	std::shared_ptr<KdBuffer> m_VB_Color = nullptr;

	//シェーダ本体
	std::shared_ptr<KdMaterial> m_material = nullptr;

};