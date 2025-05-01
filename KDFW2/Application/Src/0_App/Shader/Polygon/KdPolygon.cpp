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
	m_material = nullptr;

	m_VB_Pos = nullptr;

	m_positions.clear();
}

//描画する頂点の追加
UINT KdPolygon::AddVertex(const KdVector3& pos)
{
	//追加
	m_positions.push_back(pos);
	
	//バッファの作り直し
	//バッファ ＝ GPUに確保したメモリの事
	m_VB_Pos = nullptr;
	
	return (UINT)m_positions.size();
}

//GPU側にメモリ領域を確保してデータを転送しておく
void KdPolygon::CreateBuffers()
{
	UINT vNum = (UINT)m_positions.size();

	//バッファが確保されてなかったら確保
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
}

// 送り込んだデータを使用するようにシェーダーにセット
void KdPolygon::SetBuffers()
{
//	auto 

}

void KdPolygon::Draw()
{
}