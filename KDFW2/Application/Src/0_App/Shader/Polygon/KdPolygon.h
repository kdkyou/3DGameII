#pragma once

//指定した頂点で面を作成するクラス
class KdPolygon 
{
public:
	
	bool Initialize();
	void Release();
	void Draw();

	//描画する頂点の追加
	UINT AddVertex(const KdVector3& pos);

	//バッファの作成
	void CreateBuffers();

	//作成したバッファをシェーダーにセット
	void SetBuffers();

	//現在の頂点数を返す
//	UINT GetLength() { m_positions.size(); }

private:

	//描画する頂点の座標
	std::vector<KdVector3> m_positions;

	// バッファ
	std::shared_ptr<KdBuffer> m_VB_Pos = nullptr;

	//シェーダ本体
	std::shared_ptr<KdMaterial> m_material = nullptr;

};