#include"KdPolygon.h"
#include"PostProcess.h"

bool KdPolygon::Initialize()
{
	Release();

	//�V�F�[�_�[�̎擾
	auto shader = KdShaderManager::GetInstance().GetShader("Effect");

	//�ی���return
	if (shader == nullptr)return false;
	
	//�V�F�[�_�[����}�e���A�����쐬
	m_material = shader->CreateMaterial();



	return true;
}

void KdPolygon::Release()
{
	//�`�悪�ł��Ȃ��Ȃ�̂ň�U����Ȃ�
//	m_material = nullptr;

	m_VB_Pos = nullptr;
	m_positions.clear();

	//�ǉ��ς݂��H
	m_VB_UV = nullptr;
	m_uvs.clear();

	m_VB_Color = nullptr;
	m_colors.clear();

	//�e�N�X�`���̔j��
	m_tex = nullptr;


}

void KdPolygon::Draw(DrawType drawType)
{
	if (m_material == nullptr) { return; }

	//���_��
	uint32_t vertexNum = (uint32_t)m_positions.size();

	// �ʂƂ��Đ������Ă��Ȃ�
	// �����`�悷�邽�߂ɏ����ύX
	//if (vertexNum < 3) { return; }
	if (vertexNum < 2) { return; }

	//�o�b�t�@�̍쐬�i����Ă��Ȃ��������̂݁j
	CreateBuffers();

	//������o�b�t�@���V�F�[�_�[�ɃZ�b�g����
	SetBuffers();

	// Color�e�N�X�`�����Z�b�g����Ă��Ȃ�������
	if (m_tex == nullptr)
	{
		// ���e�N�X�`���̃Z�b�g
		SetTexture(D3D.GetDotWhiteTex());
	}
	
	//�V�F�[�_�[���Z�b�g
	m_material->SetToDevice(0);

	//�v���~�e�B�u�g�|���W�[�̃Z�b�g
	//���_�̌��ѕ�
	D3D.GetDevContext()->IASetPrimitiveTopology(
		(D3D11_PRIMITIVE_TOPOLOGY)drawType);

	//�`��R�[��
	D3D.GetDevContext()->Draw(
		vertexNum,	//�`�悷�钸�_��
		0			//�J�n�ʒu
	);
}

//�`�悷�钸�_�̒ǉ�
UINT KdPolygon::AddVertex(const KdVector3& pos, const KdVector2& uv, const uint32_t& col)
{
	//�ǉ�
	m_positions.push_back(pos);
	// 5/20
	m_uvs.push_back(uv);
	m_colors.push_back(col);

	
	//�o�b�t�@�̍�蒼��
	//�o�b�t�@ �� GPU�Ɋm�ۂ����������̎�
	m_VB_Pos = nullptr;
	// 5/20
	m_VB_UV = nullptr;
	m_VB_Color = nullptr;
	
	return (UINT)m_positions.size();
}

//GPU���Ƀ������̈���m�ۂ��ăf�[�^��]�����Ă���
void KdPolygon::CreateBuffers()
{
	UINT vNum = (UINT)m_positions.size();

	//�o�b�t�@���m�ۂ���ĂȂ�������m��

	//Position
	if (m_VB_Pos == nullptr)
	{
		m_VB_Pos = std::make_shared<KdBuffer>();
		//�m��
		m_VB_Pos->Create(
			D3D11_BIND_VERTEX_BUFFER,		//���̃����������Ɏg����
			sizeof(KdVector3) * vNum,		//�m�ۂ���T�C�Y
			D3D11_USAGE_DYNAMIC,			//�f�[�^�𑗂荞�񂾎��̃������̈�����
			0,								//�f�[�^�����ɂ��đ��鎞�̈�̃T�C�Y
			nullptr							//�������f�[�^
		);

		//��������
		m_VB_Pos->WriteData(
			&m_positions[0],			//�������ރf�[�^�̐擪�|�C���^
			m_VB_Pos->GetBufferSize()  	//�������ރT�C�Y
		);
	}

	vNum = (UINT)m_uvs.size();
	//UV
	if (m_VB_UV == nullptr)
	{
		m_VB_UV = std::make_shared<KdBuffer>();
		//�m��
		m_VB_UV->Create(D3D11_BIND_VERTEX_BUFFER,sizeof(KdVector2) * vNum,
			D3D11_USAGE_DYNAMIC,0,nullptr
		);

		//��������
		m_VB_UV->WriteData(m_uvs.data(),m_VB_UV->GetBufferSize());
	}

	//Color
	vNum = (UINT)m_colors.size();
	if (m_VB_Color == nullptr)
	{
		m_VB_Color = std::make_shared<KdBuffer>();
		//�m��
		m_VB_Color->Create(D3D11_BIND_VERTEX_BUFFER,sizeof(uint32_t) * vNum
			,D3D11_USAGE_DYNAMIC,0,nullptr);
		//��������
		m_VB_Color->WriteData(	m_colors.data(),m_VB_Color->GetBufferSize());

	}
}

// ���荞�񂾃f�[�^���g�p����悤�ɃV�F�[�_�[�ɃZ�b�g
void KdPolygon::SetBuffers()
{
	//�S�Ă̒��_���𓝍�����i����̓|�W�V�����̂݁j
	std::vector<ID3D11Buffer*> buffers;
	std::vector<uint32_t> strides;	//�e�v�f�̃T�C�Y
	std::vector<uint32_t> offsets;	//�e�v�f���ǂ�����J�n���邩

	//���W
	buffers.push_back(m_VB_Pos->GetBuffer().Get());
	strides.push_back(sizeof(KdVector3));
	offsets.push_back(0);

	//UV���i�e�N�X�`���̂ǂ����g�����j
	buffers.push_back(m_VB_UV->GetBuffer().Get());
	strides.push_back(sizeof(KdVector2));
	offsets.push_back(0);

	// Targent(�ڐ�)
	buffers.push_back(nullptr);
	strides.push_back(0);
	offsets.push_back(0);

	//Normal�i�@���j
	buffers.push_back(nullptr);
	strides.push_back(0);
	offsets.push_back(0);

	//Color
	buffers.push_back(m_VB_Color->GetBuffer().Get());
	strides.push_back(sizeof(uint32_t));
	offsets.push_back(0);

	//�V�F�[�_�[
	D3D.GetDevContext()->IASetVertexBuffers(
		0,								//�X���b�g�ԍ�
		(uint32_t)buffers.size(),		//��ނ̐�
		&buffers[0],					//�f�[�^�̖{��
		&strides[0],					//�e�v�f�̃T�C�Y	
		&offsets[0]						//�e�v�f�̊J�n�ʒu
	);

}

bool KdPolygon::SetTexture(const std::shared_ptr<KdTexture>& tex)
{
	// �e�N�X�`�����Z�b�g����}�e���A�������邩
	if (m_material == nullptr)
	{
		return false;
	}

	//�}�e���A���̃J���[�e�N�X�`���ɐݒ�
	m_material->SetTexture(0, 100, tex);

	//�N���X���̕ϐ��Ɋo���Ă���
	m_tex = tex;

	return true;

}

//void KdPolygon::Draw()
//{
//	if (m_material == nullptr) { return; }
//
//	//���_��
//	uint32_t vertexNum = (uint32_t)m_positions.size();
//
//	//�ʂƂ��Đ������Ă��Ȃ�
//	if (vertexNum < 3) { return; }
//
//	//�o�b�t�@�̍쐬�i����Ă��Ȃ��������̂݁j
//	CreateBuffers();
//
//	//������o�b�t�@���V�F�[�_�[�ɃZ�b�g����
//	SetBuffers();
//
//	//�V�F�[�_�[���Z�b�g
//	m_material->SetToDevice(0);
//
//	//�v���~�e�B�u�g�|���W�[�̃Z�b�g
//	//���_�̌��ѕ�
//	D3D.GetDevContext()->IASetPrimitiveTopology(
//		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//	//�`��R�[��
//	D3D.GetDevContext()->Draw(
//		vertexNum,	//�`�悷�钸�_��
//		0			//�J�n�ʒu
//	);
//
//}