#include "KdPolygon.h"

bool KdPolygon::Initialize()
{
	Release();

	// Shader取得
	auto shader = KdShaderManager::GetInstance().GetShader("Effect");

	// Shaderからマテリアルを作成
	m_material = shader->CreateMaterial();

	return true;
}

void KdPolygon::Release()
{
	m_VB_Pos = nullptr;
	m_VB_Color = nullptr;
	m_VB_UV = nullptr;

	m_positions.clear();
	m_colors.clear();
	m_uvs.clear();
}

// 表示頂点の追加
UINT KdPolygon::AddVertex(const KdVector3& pos, const KdVector2& uv, const uint32_t& col)
{
	m_positions.push_back(pos);
	m_colors.push_back(col);
	m_uvs.push_back(uv);

	// 追加があったらバッファを作り直す
	m_VB_Pos = nullptr;
	m_VB_Color = nullptr;
	m_VB_UV = nullptr;

	return (UINT)m_positions.size();
}

bool KdPolygon::SetTexture(const std::shared_ptr<KdTexture>& tex)
{
	if(m_material == nullptr)
	{
		return false;
	}

	// マテリアルのカラー情報に設定
	m_material->SetTexture(0, 100, tex);

	return true;
}

// 描画
void KdPolygon::Draw()
{
	// 描画準備が整っていない
	if(m_material == nullptr){return;}

	// 描画する頂点の数
	uint32_t vertexNum = (uint32_t)m_positions.size();

	// 面として成立しない
	if(vertexNum < 3){return;}

	// バッファが存在していない場合作成
	if(m_VB_Pos == nullptr)
	{
		// Positions 
		m_VB_Pos = std::make_shared<KdBuffer>();
		m_VB_Pos->Create(D3D11_BIND_VERTEX_BUFFER, sizeof(KdVector3) * vertexNum, D3D11_USAGE_DYNAMIC, 0, nullptr);
		m_VB_Pos->WriteData(m_positions.data(), m_VB_Pos->GetBufferSize());
		

		// Colors
		m_VB_Color = std::make_shared<KdBuffer>();
		m_VB_Color->Create(D3D11_BIND_VERTEX_BUFFER, sizeof(uint32_t) * vertexNum, D3D11_USAGE_DYNAMIC, 0, nullptr);
		m_VB_Color->WriteData(m_colors.data(), m_VB_Color->GetBufferSize());

		// UVs
		m_VB_UV = std::make_shared<KdBuffer>();
		m_VB_UV->Create(D3D11_BIND_VERTEX_BUFFER, sizeof(KdVector2) * vertexNum, D3D11_USAGE_DYNAMIC, 0, nullptr);
		m_VB_UV->WriteData(m_uvs.data(), m_VB_UV->GetBufferSize());
	}

	// 全ての頂点の情報を統合する
	std::vector<ID3D11Buffer*> buffers;		// バッファ本体
	std::vector<uint32_t> strides;			// 要素毎の1つ分のサイズ
	std::vector<uint32_t> offsets;			// 各要素の何番目から使うか
	{
		// Positions
		buffers.push_back( m_VB_Pos->GetBuffer().Get() );
		strides.push_back(sizeof(KdVector3));
		offsets.push_back(0);

		// Uv
		buffers.push_back(m_VB_UV->GetBuffer().Get());
		strides.push_back(sizeof(KdVector2));
		offsets.push_back(0);

		// Tangent
		buffers.push_back(nullptr);
		strides.push_back(0);
		offsets.push_back(0);

		// Normal
		buffers.push_back(nullptr);
		strides.push_back(0);
		offsets.push_back(0);

		// Colors
		buffers.push_back( m_VB_Color->GetBuffer().Get() );
		strides.push_back(sizeof(uint32_t));
		offsets.push_back(0);
	}

	// Material(Shaderをセット)
	m_material->SetToDevice(0);

	// デバイスへバッファーをセット
	D3D.GetDevContext()->IASetVertexBuffers(0, (uint32_t)buffers.size(), &buffers[0],  &strides[0], &offsets[0]);

	// プリミティブトポロジーセット
	D3D.GetDevContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 描画
	D3D.GetDevContext()->Draw(vertexNum, 0);
}
