#include "KdPolygon.h"

bool KdPolygon::Initialize()
{
	Release();

	// Shader�擾
	auto shader = KdShaderManager::GetInstance().GetShader("Effect");

	// Shader����}�e���A�����쐬
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

// �\�����_�̒ǉ�
UINT KdPolygon::AddVertex(const KdVector3& pos, const KdVector2& uv, const uint32_t& col)
{
	m_positions.push_back(pos);
	m_colors.push_back(col);
	m_uvs.push_back(uv);

	// �ǉ�����������o�b�t�@����蒼��
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

	// �}�e���A���̃J���[���ɐݒ�
	m_material->SetTexture(0, 100, tex);

	return true;
}

// �`��
void KdPolygon::Draw()
{
	// �`�揀���������Ă��Ȃ�
	if(m_material == nullptr){return;}

	// �`�悷�钸�_�̐�
	uint32_t vertexNum = (uint32_t)m_positions.size();

	// �ʂƂ��Đ������Ȃ�
	if(vertexNum < 3){return;}

	// �o�b�t�@�����݂��Ă��Ȃ��ꍇ�쐬
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

	// �S�Ă̒��_�̏��𓝍�����
	std::vector<ID3D11Buffer*> buffers;		// �o�b�t�@�{��
	std::vector<uint32_t> strides;			// �v�f����1���̃T�C�Y
	std::vector<uint32_t> offsets;			// �e�v�f�̉��Ԗڂ���g����
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

	// Material(Shader���Z�b�g)
	m_material->SetToDevice(0);

	// �f�o�C�X�փo�b�t�@�[���Z�b�g
	D3D.GetDevContext()->IASetVertexBuffers(0, (uint32_t)buffers.size(), &buffers[0],  &strides[0], &offsets[0]);

	// �v���~�e�B�u�g�|���W�[�Z�b�g
	D3D.GetDevContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �`��
	D3D.GetDevContext()->Draw(vertexNum, 0);
}
