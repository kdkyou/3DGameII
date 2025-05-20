#pragma once

//�w�肵�����_�Ŗʂ��쐬����N���X
class KdPolygon 
{
public:

	// �|���S���ɒ���t����e�N�X�`�� 5/19�ǉ�
	std::shared_ptr<KdTexture> m_tex = nullptr;
	
	bool Initialize();
	void Release();
	void Draw();

	//�`�悷�钸�_�̍��W��UV��Color��ǉ�
	UINT AddVertex(const KdVector3& pos,const KdVector2& uv, const uint32_t& col= 0xFFFFFFFF);

	//�o�b�t�@�̍쐬
	void CreateBuffers();

	//�쐬�����o�b�t�@���V�F�[�_�[�ɃZ�b�g
	void SetBuffers();

	//���݂̒��_����Ԃ�
//	UINT GetLength() { m_positions.size(); }

	//�O������摜���󂯎��
	bool SetTexture(const std::string& path);

private:

	//�`�悷�钸�_�̍��W
	std::vector<KdVector3> m_positions;
	//���_��UV���W	5/20
	std::vector<KdVector2> m_uvs;
	//���_�̐F		5/20
	std::vector<uint32_t> m_colors;


	// �o�b�t�@
	std::shared_ptr<KdBuffer> m_VB_Pos = nullptr;
	//	5/20
	std::shared_ptr<KdBuffer> m_VB_UV = nullptr;
	std::shared_ptr<KdBuffer> m_VB_Color = nullptr;

	//�V�F�[�_�{��
	std::shared_ptr<KdMaterial> m_material = nullptr;

};