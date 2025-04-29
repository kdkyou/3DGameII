#pragma once

//==========================================================
// ���b�V���V�[���f�[�^
// �E�����̃��b�V���Ȃǂ��W�܂�������
//==========================================================

struct KdAnimationData; 

class KdMeshScene
{
public :

	// ���_�f�[�^
	struct VertexData
	{
		// ���_�f�[�^���X�g
		std::vector<KdVector3>			PosList;			// ���W���X�g
		std::vector<KdVector2>			UVList;				// UV���W���X�g
		std::vector<KdVector3>			TangentList;		// �ڐ����X�g
		std::vector<KdVector3>			NormalList;			// �@�����X�g
		std::vector<uint32_t>			ColorList;			// �F���X�g

		std::vector<std::array<short, 4>>	SkinIndexList;	// �X�L�j���OIndex���X�g
		std::vector<std::array<float, 4>>	SkinWeightList;	// �X�L�j���O�E�F�C�g���X�g

		// KdMeshVertexData����������
		void Combine(const VertexData& vertex)
		{
			PosList.insert(PosList.end(), vertex.PosList.begin(), vertex.PosList.end());
			UVList.insert(UVList.end(), vertex.UVList.begin(), vertex.UVList.end());
			TangentList.insert(TangentList.end(), vertex.TangentList.begin(), vertex.TangentList.end());
			NormalList.insert(NormalList.end(), vertex.NormalList.begin(), vertex.NormalList.end());
			ColorList.insert(ColorList.end(), vertex.ColorList.begin(), vertex.ColorList.end());

			SkinIndexList.insert(SkinIndexList.end(), vertex.SkinIndexList.begin(), vertex.SkinIndexList.end());
			SkinWeightList.insert(SkinWeightList.end(), vertex.SkinWeightList.begin(), vertex.SkinWeightList.end());
		}

		void Clear()
		{
			PosList.clear();
			UVList.clear();
			TangentList.clear();
			NormalList.clear();
			ColorList.clear();
			SkinIndexList.clear();
			SkinWeightList.clear();
		}
	};
	// ��(Index)���
	struct FaceData
	{
		uint32_t Idx[3];				// �O�p�`���\�����钸�_��Index
	};
	/*
	// �T�u�Z�b�g���
	struct SubsetData
	{
		uint32_t		MaterialNo = 0;		// �}�e���A��No

		uint32_t		FaceStart = 0;		// ��Index�@���̃}�e���A���Ŏg�p����Ă���ŏ��̖ʂ�Index
		uint32_t		FaceCount = 0;		// �ʐ��@FaceStart����A�����̖ʂ��g�p����Ă��邩��
	};
	*/

	// �}�e���A���f�[�^
	struct MaterialData
	{
		// ��{���
		std::string Name;

		// ��{�e�N�X�`���t�@�C����
		std::string BaseMapFilename;
		// ��{�F
		Math::Color BaseColor;

		// �@���}�b�v�t�@�C����
		std::string NormalMapFilename;

		// ����
		float Metallic = 0;
		// ���炩��
		float Smoothness = 0;

		// ���ːF�}�b�v�t�@�C����
		std::string EmissiveMapFilename;
		// ���ːF
		Math::Color EmissiveColor;

	};

	// ���b�V���f�[�^(�}�e���A���P�ʂŕ�������Ă���)
	struct MeshData
	{
		// ���b�V����
		std::string				Name;
		// ���_�f�[�^
		VertexData				Vertex;
		// ��(Index)�f�[�^
		std::vector<FaceData>	FaceDatas;
		// �T�u�Z�b�g���
//		std::vector<SubsetData>	Subset;

		// �}�e���A���f�[�^
//		MaterialData			material;
		// �}�e���A���z�����Index
		uint32_t				MaterialIndex;

		void Clear()
		{
			Name = "";
			Vertex.Clear();
			FaceDatas.clear();
			MaterialIndex = 0;
//			Subset.clear();
		}
	};

	// �}�e���A���f�[�^
	std::vector<MaterialData>	m_materials;

	// AnimationData
	std::vector<KdAnimationData> m_animations;

	// �m�[�h�f�[�^
	struct NodeData
	{
		// ���O
		std::string				NodeName;

		// ���m�[�hIndex
		int						NodeIndex = -1;
		int						BoneIndex = -1;

		// �qIndex���X�g
		std::vector<int>		Children;
		// �eIndex
		int						ParentIndex = -1;

		// ���[�J���s��
		KdMatrix					LocalTransform;

		// �{�[���I�t�Z�b�g�s��
		KdMatrix					BoneOffsetMatrix;

		// ���b�V�����X�g
		std::vector<MeshData>	m_meshDatas;
	};



public:

	// ���b�V���̃p�X
	std::filesystem::path	m_filepath;

	// �S�m�[�h
	std::vector<NodeData>	m_allNodes;

	// �S�m�[�h���A�{�[���ł���m�[�hIndex�W
	std::vector<uint32_t>	m_boneIndices;
	// �S�m�[�h���A���b�V���̂���m�[�hIndex�W
	std::vector<uint32_t>	m_meshIndices;


	// ���b�V���t�@�C����ǂݍ���
	bool LoadMesh(const std::string& filename);

	void Clear()
	{
		m_filepath = "";
		m_allNodes.clear();
		m_boneIndices.clear();
		m_meshIndices.clear();
	}

};