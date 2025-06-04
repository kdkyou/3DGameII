#include"KdPolygon.h"
#include"PostProcess.h"

bool KdPolygon::Initialize()
{
	Release();

	//シェーダーの取得
	auto shader = KdShaderManager::GetInstance().GetShader("Effect");

	//保険のreturn
	if (shader == nullptr)return false;
	
	//シェーダーからマテリアルを作成
	m_material = shader->CreateMaterial();



	return true;
}

void KdPolygon::Release()
{
	//描画ができなくなるので一旦解放なし
//	m_material = nullptr;

	m_VB_Pos = nullptr;
	m_positions.clear();

	//追加済みか？
	m_VB_UV = nullptr;
	m_uvs.clear();

	m_VB_Color = nullptr;
	m_colors.clear();

	//テクスチャの破棄
	m_tex = nullptr;


}

void KdPolygon::Draw(DrawType drawType)
{
	if (m_material == nullptr) { return; }

	//頂点数
	uint32_t vertexNum = (uint32_t)m_positions.size();

	// 面として成立していない
	// 線も描画するために条件変更
	//if (vertexNum < 3) { return; }
	if (vertexNum < 2) { return; }

	//バッファの作成（作っていなかった時のみ）
	CreateBuffers();

	//作ったバッファをシェーダーにセットする
	SetBuffers();

	// Colorテクスチャがセットされていなかったら
	if (m_tex == nullptr)
	{
		// 白テクスチャのセット
		SetTexture(D3D.GetDotWhiteTex());
	}
	
	//シェーダーをセット
	m_material->SetToDevice(0);

	//プリミティブトポロジーのセット
	//頂点の結び方
	D3D.GetDevContext()->IASetPrimitiveTopology(
		(D3D11_PRIMITIVE_TOPOLOGY)drawType);

	//描画コール
	D3D.GetDevContext()->Draw(
		vertexNum,	//描画する頂点数
		0			//開始位置
	);
}

//描画する頂点の追加
UINT KdPolygon::AddVertex(const KdVector3& pos, const KdVector2& uv, const uint32_t& col)
{
	//追加
	m_positions.push_back(pos);
	// 5/20
	m_uvs.push_back(uv);
	m_colors.push_back(col);

	
	//バッファの作り直し
	//バッファ ＝ GPUに確保したメモリの事
	m_VB_Pos = nullptr;
	// 5/20
	m_VB_UV = nullptr;
	m_VB_Color = nullptr;
	
	return (UINT)m_positions.size();
}

//GPU側にメモリ領域を確保してデータを転送しておく
void KdPolygon::CreateBuffers()
{
	UINT vNum = (UINT)m_positions.size();

	//バッファが確保されてなかったら確保

	//Position
	if (m_VB_Pos == nullptr)
	{
		m_VB_Pos = std::make_shared<KdBuffer>();
		//確保
		m_VB_Pos->Create(
			D3D11_BIND_VERTEX_BUFFER,		//このメモリを何に使うか
			sizeof(KdVector3) * vNum,		//確保するサイズ
			D3D11_USAGE_DYNAMIC,			//データを送り込んだ時のメモリの扱い方
			0,								//データを一塊にして送る時の一つのサイズ
			nullptr							//初期化データ
		);

		//書き込み
		m_VB_Pos->WriteData(
			&m_positions[0],			//書き込むデータの先頭ポインタ
			m_VB_Pos->GetBufferSize()  	//書き込むサイズ
		);
	}

	vNum = (UINT)m_uvs.size();
	//UV
	if (m_VB_UV == nullptr)
	{
		m_VB_UV = std::make_shared<KdBuffer>();
		//確保
		m_VB_UV->Create(D3D11_BIND_VERTEX_BUFFER,sizeof(KdVector2) * vNum,
			D3D11_USAGE_DYNAMIC,0,nullptr
		);

		//書き込み
		m_VB_UV->WriteData(m_uvs.data(),m_VB_UV->GetBufferSize());
	}

	//Color
	vNum = (UINT)m_colors.size();
	if (m_VB_Color == nullptr)
	{
		m_VB_Color = std::make_shared<KdBuffer>();
		//確保
		m_VB_Color->Create(D3D11_BIND_VERTEX_BUFFER,sizeof(uint32_t) * vNum
			,D3D11_USAGE_DYNAMIC,0,nullptr);
		//書き込み
		m_VB_Color->WriteData(	m_colors.data(),m_VB_Color->GetBufferSize());

	}
}

// 送り込んだデータを使用するようにシェーダーにセット
void KdPolygon::SetBuffers()
{
	//全ての頂点情報を統合する（今回はポジションのみ）
	std::vector<ID3D11Buffer*> buffers;
	std::vector<uint32_t> strides;	//各要素のサイズ
	std::vector<uint32_t> offsets;	//各要素をどこから開始するか

	//座標
	buffers.push_back(m_VB_Pos->GetBuffer().Get());
	strides.push_back(sizeof(KdVector3));
	offsets.push_back(0);

	//UV情報（テクスチャのどこを使うか）
	buffers.push_back(m_VB_UV->GetBuffer().Get());
	strides.push_back(sizeof(KdVector2));
	offsets.push_back(0);

	// Targent(接線)
	buffers.push_back(nullptr);
	strides.push_back(0);
	offsets.push_back(0);

	//Normal（法線）
	buffers.push_back(nullptr);
	strides.push_back(0);
	offsets.push_back(0);

	//Color
	buffers.push_back(m_VB_Color->GetBuffer().Get());
	strides.push_back(sizeof(uint32_t));
	offsets.push_back(0);

	//シェーダー
	D3D.GetDevContext()->IASetVertexBuffers(
		0,								//スロット番号
		(uint32_t)buffers.size(),		//種類の数
		&buffers[0],					//データの本体
		&strides[0],					//各要素のサイズ	
		&offsets[0]						//各要素の開始位置
	);

}

bool KdPolygon::SetTexture(const std::shared_ptr<KdTexture>& tex)
{
	// テクスチャをセットするマテリアルがあるか
	if (m_material == nullptr)
	{
		return false;
	}

	//マテリアルのカラーテクスチャに設定
	m_material->SetTexture(0, 100, tex);

	//クラス内の変数に覚えておく
	m_tex = tex;

	return true;

}

//void KdPolygon::Draw()
//{
//	if (m_material == nullptr) { return; }
//
//	//頂点数
//	uint32_t vertexNum = (uint32_t)m_positions.size();
//
//	//面として成立していない
//	if (vertexNum < 3) { return; }
//
//	//バッファの作成（作っていなかった時のみ）
//	CreateBuffers();
//
//	//作ったバッファをシェーダーにセットする
//	SetBuffers();
//
//	//シェーダーをセット
//	m_material->SetToDevice(0);
//
//	//プリミティブトポロジーのセット
//	//頂点の結び方
//	D3D.GetDevContext()->IASetPrimitiveTopology(
//		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//	//描画コール
//	D3D.GetDevContext()->Draw(
//		vertexNum,	//描画する頂点数
//		0			//開始位置
//	);
//
//}