#include "KdMeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

bool KdMeshScene::LoadMesh(const std::string& filename)
{
	Clear();

	m_filepath = filename;

	/*
	// ���^�t�@�C��
	{
		nlohmann::json metaData;
		std::ifstream ifs(filename + ".kdfwmeta");
		if (ifs)
		{
			ifs >> metaData;
		}
	}
	*/

	//--------------------------------
	// ���f�����[�h(assimp)
	//--------------------------------
	Assimp::Importer importer;
	uint32_t flag = aiProcess_Triangulate |
					aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder |
					aiProcess_CalcTangentSpace | aiProcess_FlipUVs;
	auto pScene = importer.ReadFile(filename, flag);
	if (pScene == nullptr)
	{
		m_filepath = "";
		return false;
	}

	// ���ƂŃ��b�V�����쐬����p�̉��f�[�^
	struct TempCreateMeshData
	{
		aiNode*		SrcNode = nullptr;
		int			DestNodeIndex = 0;
	};
	std::vector<TempCreateMeshData> tempCreateMeshDatas;

	//--------------------------------
	// �}�e���A�����X�g
	//--------------------------------
	m_materials.resize(pScene->mNumMaterials);
	for (uint32_t iMate = 0; iMate < pScene->mNumMaterials; iMate++)
	{
		auto pMate = pScene->mMaterials[iMate];

		MaterialData& destMaterial = m_materials[iMate];

		std::vector<std::string> keys;
		for (uint32_t i = 0; i < pMate->mNumProperties; i++)
		{
			auto prop = pMate->mProperties[i];
			keys.push_back(prop->mKey.C_Str());
		}

		// ���O
		aiString name;
		if (pMate->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
		{
			destMaterial.Name = name.C_Str();
		}

		// ��{�e�N�X�`����
		aiString path;
		if (pMate->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
		{
			destMaterial.BaseMapFilename = path.C_Str();
		}

		// ��{�F
		aiColor4D color;
		if (pMate->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
		{
			destMaterial.BaseColor = { color.r, color.g, color.b, color.a };
		}
		float alpha = 1;
		if (pMate->Get(AI_MATKEY_OPACITY, alpha) == AI_SUCCESS)
		{
			destMaterial.BaseColor.w = alpha;
		}

		// �@���}�b�v
		if (pMate->Get(AI_MATKEY_TEXTURE_NORMALS(0), path) == AI_SUCCESS)
		{
			destMaterial.NormalMapFilename = path.C_Str();
		}

		// ���˃}�b�v
		if (pMate->Get(AI_MATKEY_TEXTURE_EMISSIVE(0), path) == AI_SUCCESS)
		{
			destMaterial.EmissiveMapFilename = path.C_Str();
		}
		if (pMate->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
		{
			destMaterial.EmissiveColor = { color.r, color.g, color.b, color.a };
		}

		/*
		// ���炩�������e�N�X�`����(���^�t�@�C������)
		auto ary = metaData["Materials"];
		for (auto&& mate : ary)
		{
			if (mate["Name"].get<std::string>() == outMeshData.material.Name)
			{
				auto j = mate["SmoothnessMetallicMapFilename"];
				if (j != nullptr)
				{
					outMeshData.material.SmoothnessMetallicMapFilename = j.get<std::string>();
				}
				break;
			}
		}
		*/

		/*
		// �V���C�l�X
		float shininess = 0;
		if (pMate->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
		{
			destMaterial.Smoothness = shininess;
		}
		// ���^���b�N
		float metallic = 0;
		if (pMate->Get(AI_MATKEY_REFLECTIVITY, metallic) == AI_SUCCESS)
		{
			destMaterial.Metallic = metallic;
		}
		*/
	}

	//--------------------------------
	// �S�m�[�h�쐬
	//--------------------------------
	std::function<void(aiNode*, int)> RecNode = [this, &RecNode, &tempCreateMeshDatas](aiNode* node, int parentNodeIndex)
		{
			int nodeIndex = (int)m_allNodes.size();

			NodeData nodeData;
			nodeData.NodeName = node->mName.C_Str();
			nodeData.NodeIndex = nodeIndex;
			nodeData.ParentIndex = parentNodeIndex;
			nodeData.LocalTransform = *(KdMatrix*)&node->mTransformation.Transpose();
			m_allNodes.push_back(nodeData);

			if (parentNodeIndex >= 0)
			{
				m_allNodes[parentNodeIndex].Children.push_back(nodeIndex);
			}

			// ���b�V���쐬
			if (node->mNumMeshes > 0)
			{
				// ���b�V���͌�ō쐬���邽�߁B�����L�����Ă���
				tempCreateMeshDatas.push_back({ node, nodeIndex });

				// ���b�V����Index���L��
				m_meshIndices.push_back(nodeIndex);
			}

			// �q�ċA
			for (uint32_t iChild = 0; iChild < node->mNumChildren; iChild++)
			{
				RecNode(node->mChildren[iChild], nodeIndex);
			}
		};

	RecNode(pScene->mRootNode, -1);

	//--------------------------------
	// ���b�V���f�[�^���쐬
	//--------------------------------
	for (auto&& createNodeData : tempCreateMeshDatas)
	{
		// ���f�[�^
		aiNode* srcNode = createNodeData.SrcNode;
		// �ۑ���f�[�^
		NodeData* destNodeData = &m_allNodes[createNodeData.DestNodeIndex];

		// ���b�V����
		if (srcNode->mNumMeshes > 0)
		{
			// ���b�V���������쐬
			destNodeData->m_meshDatas.resize(srcNode->mNumMeshes);

			// �S���b�V��
			for (uint32_t mi = 0; mi < srcNode->mNumMeshes; mi++)
			{
				uint32_t srcMeshIdx = srcNode->mMeshes[mi];

				// �����b�V���f�[�^
				aiMesh* pMeshData = pScene->mMeshes[srcMeshIdx];
				// �i�[��f�[�^�̎Q��
				auto& outMeshData = destNodeData->m_meshDatas[mi];

				// ���O (�������O�̃��b�V���́A���ƂŌ����Ƃ����Ă��ǂ�)
				destNodeData->m_meshDatas[mi].Name = pMeshData->mName.C_Str();

				//---------------------------------------------------
				// ���_�f�[�^
				//---------------------------------------------------
				outMeshData.Vertex.PosList.resize(pMeshData->mNumVertices);
				if (pMeshData->HasNormals()) outMeshData.Vertex.NormalList.resize(pMeshData->mNumVertices);
				if (pMeshData->HasTangentsAndBitangents()) outMeshData.Vertex.TangentList.resize(pMeshData->mNumVertices);
				if (pMeshData->HasVertexColors(0)) outMeshData.Vertex.ColorList.resize(pMeshData->mNumVertices);
				if (pMeshData->HasTextureCoords(0)) outMeshData.Vertex.UVList.resize(pMeshData->mNumVertices);

				for (uint32_t iV = 0; iV < pMeshData->mNumVertices; iV++)
				{
					// ���W
					outMeshData.Vertex.PosList[iV] = { pMeshData->mVertices[iV].x, pMeshData->mVertices[iV].y, pMeshData->mVertices[iV].z };

					// �@��
					if (pMeshData->HasNormals())
					{
						outMeshData.Vertex.NormalList[iV] = { pMeshData->mNormals[iV].x, pMeshData->mNormals[iV].y, pMeshData->mNormals[iV].z };
					}
					// �ڐ�
					if (pMeshData->HasTangentsAndBitangents())
					{
						outMeshData.Vertex.TangentList[iV] = { pMeshData->mTangents[iV].x, pMeshData->mTangents[iV].y, pMeshData->mTangents[iV].z };
					}
					// �F
					if (pMeshData->HasVertexColors(0))
					{
						DirectX::SimpleMath::Color c(
							pMeshData->mColors[0][iV].r,
							pMeshData->mColors[0][iV].g,
							pMeshData->mColors[0][iV].b,
							pMeshData->mColors[0][iV].a
						);

						outMeshData.Vertex.ColorList[iV] = c.RGBA().v;
					}
					// UV
					if (pMeshData->HasTextureCoords(0))
					{
						outMeshData.Vertex.UVList[iV] = { pMeshData->mTextureCoords[0][iV].x, pMeshData->mTextureCoords[0][iV].y };
					}
				}

				// �ʃf�[�^
				outMeshData.FaceDatas.resize(pMeshData->mNumFaces);
				for (uint32_t i = 0; i < pMeshData->mNumFaces; ++i)
				{
					const auto& face = pMeshData->mFaces[i];

					outMeshData.FaceDatas[i].Idx[0] = face.mIndices[0];
					outMeshData.FaceDatas[i].Idx[1] = face.mIndices[1];
					outMeshData.FaceDatas[i].Idx[2] = face.mIndices[2];
				}

				// �{�[��
				if (pMeshData->HasBones())
				{
					// ���_�̃E�F�C�g�֌W�̏����쐬
					if (outMeshData.Vertex.SkinWeightList.size() == 0)
					{
						outMeshData.Vertex.SkinWeightList.resize(pMeshData->mNumVertices);
						outMeshData.Vertex.SkinIndexList.resize(pMeshData->mNumVertices);
						for (auto&& p : outMeshData.Vertex.SkinIndexList)
						{
							p[0] = p[1] = p[2] = p[3] = -1;
						}
					}

					// �S�{�[��
					m_boneIndices.resize(pMeshData->mNumBones);
					for (uint32_t iBone = 0; iBone < pMeshData->mNumBones; iBone++)
					{
						aiBone* srcBone = pMeshData->mBones[iBone];

						// �{�[��������A�m�[�h������
						std::string boneName = srcBone->mName.C_Str();

						auto itFoundNode = std::find_if(m_allNodes.begin(), m_allNodes.end(), [boneName](const NodeData& nodeData) {return nodeData.NodeName == boneName; });
						if (itFoundNode != m_allNodes.end())
						{
							m_boneIndices[iBone] = (uint32_t)std::distance(m_allNodes.begin(), itFoundNode);

							// �m�[�h�����A��������
							itFoundNode->BoneIndex = (int)iBone;
							itFoundNode->BoneOffsetMatrix = *(KdMatrix*)&srcBone->mOffsetMatrix.Transpose();

							// ���̃{�[���ɉe�����Ă��钸�_�ɃE�F�C�g�����쐬
							for (uint32_t iW = 0; iW < srcBone->mNumWeights; iW++)
							{
								// �Ώۂ̒��_Index
								uint32_t vertexIdx = srcBone->mWeights[iW].mVertexId;

								auto& destBoneIdxArray = outMeshData.Vertex.SkinIndexList[vertexIdx];
								auto& destBoneWeightArray = outMeshData.Vertex.SkinWeightList[vertexIdx];

								// �w�蒸�_�̋󂫃E�F�C�g�ꏊ��T���A�����ɒl�������
								for (size_t iDest = 0; iDest < destBoneIdxArray.size(); iDest++)
								{
									// �󂢂Ă��邩�H
									if (destBoneIdxArray[iDest] == -1)
									{
										// ���̒��_���e������{�[���ԍ�
										destBoneIdxArray[iDest] = iBone;
										// �E�F�C�g�l
										destBoneWeightArray[iDest] = srcBone->mWeights[iW].mWeight;
										break;
									}
								}
							}
						}

					}
				}

				// ���_�E�F�C�g�𐳋K��
				for (uint32_t iV = 0; iV < outMeshData.Vertex.SkinIndexList.size(); iV++)
				{
					uint32_t numWeights = 0;
					float totalWeight = 0;
					// �E�F�C�gLoop
					for (numWeights = 0; numWeights < outMeshData.Vertex.SkinIndexList[iV].size(); numWeights++)
					{
						// �E�F�C�g���v�����߂�
						if (outMeshData.Vertex.SkinIndexList[iV][numWeights])
						{
							totalWeight += outMeshData.Vertex.SkinWeightList[iV][numWeights];
						}
					}

					// �Ō�̃E�F�C�g�𒲐�
					for (uint32_t idx = 0; idx < numWeights; idx++)
					{
						outMeshData.Vertex.SkinWeightList[iV][idx] /= totalWeight;
					}
					/*
					if (numWeights >= 1)
					{
						outMeshData.Vertex.SkinWeightList[iV][numWeights - 1] = 1.0f - totalWeight;
					}
					*/
				}

				// �}�e���A��Index
				outMeshData.MaterialIndex = pMeshData->mMaterialIndex;

				/*
				//---------------------------------------------------
				// �}�e���A���f�[�^
				//---------------------------------------------------
				auto pMate = pScene->mMaterials[pMeshData->mMaterialIndex];


				std::vector<std::string> keys;
				for (uint32_t i = 0; i < pMate->mNumProperties; i++)
				{
					auto prop = pMate->mProperties[i];
					keys.push_back(prop->mKey.C_Str());
				}

				// ���O
				aiString name;
				if (pMate->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
				{
					outMeshData.material.Name = name.C_Str();
				}

				// ��{�e�N�X�`����
				aiString path;
				if (pMate->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)
				{
					outMeshData.material.BaseMapFilename = path.C_Str();
				}

				// ��{�F
				aiColor4D color;
				if (pMate->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
				{
					outMeshData.material.BaseColor = { color.r, color.g, color.b, color.a };
				}
				float alpha = 1;
				if (pMate->Get(AI_MATKEY_OPACITY, alpha) == AI_SUCCESS)
				{
					outMeshData.material.BaseColor.w = alpha;
				}

				// �@���}�b�v
				if (pMate->Get(AI_MATKEY_TEXTURE_NORMALS(0), path) == AI_SUCCESS)
				{
					outMeshData.material.NormalMapFilename = path.C_Str();
				}

				// ���˃}�b�v
				if (pMate->Get(AI_MATKEY_TEXTURE_EMISSIVE(0), path) == AI_SUCCESS)
				{
					outMeshData.material.EmissiveMapFilename = path.C_Str();
				}
				if (pMate->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS)
				{
					outMeshData.material.EmissiveColor = { color.r, color.g, color.b, color.a };
				}

				// �V���C�l�X
				float shininess = 0;
				if (pMate->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
				{
					outMeshData.material.Smoothness = shininess;
				}
				// ���^���b�N
				float metallic = 0;
				if (pMate->Get(AI_MATKEY_REFLECTIVITY, metallic) == AI_SUCCESS)
				{
					outMeshData.material.Metallic = metallic;
				}
				*/
			}

		}
	}

	// Animation
	if( pScene->HasAnimations() )
	{
		UINT animNum = pScene->mNumAnimations; // �A�j���[�V�����̐�

		for(UINT cnt = 0; cnt < animNum; cnt++)
		{
			KdAnimationData destAnim;
			destAnim.Parse(pScene->mAnimations[cnt], m_allNodes);
			m_animations.push_back(destAnim);
		}
	}

	return true;
}