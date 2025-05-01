#pragma once

//�w�肵�����_�Ŗʂ��쐬����N���X
class KdPolygon 
{
public:
	
	bool Initialize();
	void Release();
	void Draw();

	//�`�悷�钸�_�̒ǉ�
	UINT AddVertex(const KdVector3& pos);

	//�o�b�t�@�̍쐬
	void CreateBuffers();

	//�쐬�����o�b�t�@���V�F�[�_�[�ɃZ�b�g
	void SetBuffers();

	//���݂̒��_����Ԃ�
//	UINT GetLength() { m_positions.size(); }

private:

	//�`�悷�钸�_�̍��W
	std::vector<KdVector3> m_positions;

	// �o�b�t�@
	std::shared_ptr<KdBuffer> m_VB_Pos = nullptr;

	//�V�F�[�_�{��
	std::shared_ptr<KdMaterial> m_material = nullptr;

};