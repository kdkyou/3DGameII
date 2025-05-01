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
	m_material = nullptr;

	m_VB_Pos = nullptr;

	m_positions.clear();
}

//�`�悷�钸�_�̒ǉ�
UINT KdPolygon::AddVertex(const KdVector3& pos)
{
	//�ǉ�
	m_positions.push_back(pos);
	
	//�o�b�t�@�̍�蒼��
	//�o�b�t�@ �� GPU�Ɋm�ۂ����������̎�
	m_VB_Pos = nullptr;
	
	return (UINT)m_positions.size();
}

//GPU���Ƀ������̈���m�ۂ��ăf�[�^��]�����Ă���
void KdPolygon::CreateBuffers()
{
	UINT vNum = (UINT)m_positions.size();

	//�o�b�t�@���m�ۂ���ĂȂ�������m��
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
}

// ���荞�񂾃f�[�^���g�p����悤�ɃV�F�[�_�[�ɃZ�b�g
void KdPolygon::SetBuffers()
{
//	auto 

}

void KdPolygon::Draw()
{
}