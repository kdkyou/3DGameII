#pragma once

#include "KdComponent.h"
#include "KdTransformComponent.h"

#if 0
class KdMeshRenderer : public KdRendererComponent
{
public:

	// KdModel��ݒ�
	// model		 : KdModel
	// meshNodeName	 : ���b�V���m�[�h��
	void SetModel(std::shared_ptr<KdModel> model, const std::string& meshNodeName)
	{
		Release();
		if (model == nullptr)return;

		// ���f���f�[�^
		m_sourceModel = model;

		// ���f�����̃��b�V���m�[�h��
		m_nodeName = meshNodeName;

		// �m�[�h�����烁�b�V���m�[�h������
		if (model != nullptr)
		{
			const KdModel::Node* node = model->GetNode(m_nodeName);
			if (node != nullptr && node->Mesh)
			{
				// ���b�V����ݒ�
				SetMesh(node->Mesh, &model->GetMaterials());

				// �X�L�����b�V��
				if (node->Mesh->IsSkinMesh())
				{

				}
			}
		}

	}

	// KdMesh�ƃ}�e���A����ݒ�
	void SetMesh(std::shared_ptr<KdMesh> mesh, const std::vector<std::shared_ptr<KdMaterial>>* srcMaterials)
	{
		// ���b�V��
		m_mesh = mesh;

		// �T�u�Z�b�g���Ԃ�A�}�e���A���z���p��
		m_materials.resize(m_mesh->GetSubsets().size());
		if (srcMaterials != nullptr)
		{
			// �T�u�Z�b�g�ɑΉ������}�e���A�����擾����
			for (uint32_t iSubset = 0; iSubset < m_mesh->GetSubsets().size(); iSubset++)
			{
				// �T�u�Z�b�g���
				const KdMeshSubset& subset = m_mesh->GetSubsets()[iSubset];

				// �}�e���A��Index���͈͊O�Ȃ�A�X�L�b�v
				if (subset.MaterialNo >= srcMaterials->size())continue;

				// �}�e���A���o�^
				m_materials[iSubset] = (*srcMaterials)[subset.MaterialNo];
			}
		}
	}

	// �`��O����
	virtual void PreDraw() override;

	// �`�揈��
	virtual void Draw(bool opaque, KdShader::PassTags passTag) override;

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		// �p�����̊֐������s
		KdComponent::Deserialize(jsonObj);

		// �A�Z�b�g�p�X���烂�f�������[�h
		std::string assetPath;
		GetFromJson(jsonObj, "m_assetPath", assetPath);

		auto model = KdResourceManager::GetInstance().LoadAsset<KdModel>(assetPath);

		std::string nodeName;
		GetFromJson(jsonObj, "m_nodeName", nodeName);

		SetModel(model, nodeName);
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(nlohmann::json& outJson) const
	{
		// �p�����̊֐������s
		KdComponent::Serialize(outJson);

		// �A�Z�b�g�p�X
		if (m_sourceModel != nullptr)
		{
			outJson["m_assetPath"] = m_sourceModel->GetAssetPath();
		}
		else
		{
			outJson["m_assetPath"] = "";
		}

		// �m�[�h��
		outJson["m_nodeName"] = m_nodeName;

		// �}�e���A�����

	}

	virtual void Editor_ImGui() override;

	void Release()
	{
		m_sourceModel = nullptr;
		m_mesh = nullptr;
		m_nodeName = "";
		m_materials.clear();

		m_bones.clear();

//		m_cbBoneMatrices = nullptr;
	}

private:

	// �����f���f�[�^(�A�Z�b�g���烍�[�h��)
	std::shared_ptr<KdModel>		m_sourceModel;

	// ���b�V���̂���m�[�h��
	std::string						m_nodeName;

	// ���b�V��
	std::shared_ptr<KdMesh>			m_mesh;

	// ���̃��b�V���p�̃}�e���A�����X�g
	std::vector<std::shared_ptr<KdMaterial>>	m_materials;

	// (SkinMesh)�{�[��Transform�z��
	std::vector<std::shared_ptr<KdTransformComponent>>	m_bones;

};
#endif

class KdModelRendererComponent : public KdRendererComponent
{
public:

	// �m�[�h���������̃m�[�h�C���f�b�N�X��Ԃ�
	int GetNodeIndexFromName(const std::string& nodeName)const;


	void Load();

	void SetMode(const std::shared_ptr<KdModel>& model);
	const std::shared_ptr<KdModel> GetModel(){return m_sourceModel;}

	// �`��O����
	virtual void PreDraw() override;

	// �`�揈��
	virtual void Draw(bool opaque, KdShader::PassTags passTag) override;

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const nlohmann::json& jsonObj)
	{
		// �p�����̊֐������s
		KdComponent::Deserialize(jsonObj);

		// �A�Z�b�g�p�X���烂�f�������[�h
		std::string assetPath;
		KdJsonUtility::GetValue(jsonObj, "m_assetPath", &assetPath);

		auto model = KdResourceManager::GetInstance().LoadAsset<KdModel>(assetPath);

		SetMode(model);

		// �}�e���A�����
		KdJsonUtility::GetArray(jsonObj, "Materials", [this](uint32_t idx, nlohmann::json jsonObj)
			{
				// �}�e���A���𕡐�
				m_materials[idx] = m_materials[idx]->Clone();
				// �f�[�^����
				m_materials[idx]->Deserialize(jsonObj);
			});
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(nlohmann::json& outJson) const
	{
		// �p�����̊֐������s
		KdComponent::Serialize(outJson);

		// �A�Z�b�g�p�X
		if (m_sourceModel != nullptr)
		{
			outJson["m_assetPath"] = m_sourceModel->GetAssetPath();
		}
		else
		{
			outJson["m_assetPath"] = "";
		}

		// �}�e���A�����
		auto materialAry = nlohmann::json::array();
		for (auto&& mate : m_materials)
		{
			if (mate->IsInstanced())
			{
				nlohmann::json js;
				mate->Serialize(js);

				materialAry.push_back(js);
			}
		}
		outJson["Materials"] = materialAry;
	}

	// ImGui
	virtual void Editor_ImGui() override;

	// 
	void Release()
	{
		m_sourceModel = nullptr;
		m_allNodeTransforms.clear();
		m_materials.clear();
		m_cbBoneMatrices = nullptr;

		m_gpuSkinningData.clear();

	}


	const std::vector<KdTransform>& GetAllNodeTransforms()const {return m_allNodeTransforms;}
	void SetAnimatedTransform( const std::shared_ptr<std::vector<KdTransform>>& animatedTransform){m_animatedTransform = animatedTransform;}

	const std::shared_ptr<KdModel> GetSourceModel() const {return m_sourceModel;}


private:

	// ���f���f�[�^
	std::shared_ptr<KdModel>					m_sourceModel;

	// �S�m�[�h���W���X�g
	std::vector<KdTransform>					m_allNodeTransforms;

	// �A�j���[�V��������Transform(����null�N���A)
	std::weak_ptr<std::vector<KdTransform>>		m_animatedTransform;

	// ���̃��b�V���p�̃}�e���A�����X�g
	std::vector<std::shared_ptr<KdMaterial>>	m_materials;

	// (SkinMesh)�{�[���s��p �萔�o�b�t�@
	std::shared_ptr<KdConstantBufferType<KdSkinningShader::CBufferData>>	m_cbBoneMatrices;

	// (SkinMesh)�{�[�����O�X�L�j���O�p ���_�o�b�t�@�W
	std::vector<std::shared_ptr<KdMeshGPUSkinningData>>	m_gpuSkinningData;

};