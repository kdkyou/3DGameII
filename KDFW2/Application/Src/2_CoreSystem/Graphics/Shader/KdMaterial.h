#pragma once

#include "KdShader.h"

//============================================
// 
// �}�e���A��
// 
//============================================
class KdMaterial
{
public:

	// �P�p�X�̏��
	// �E�V�F�[�_�[(VS,PS,GS�Ȃ�)�A�萔�o�b�t�@�A�e�N�X�`��
	struct Pass
	{
		bool									m_opaque = true;		// �s����

		// �p�XIndex
		int32_t									m_passIndex = -1;

		// �}�e���A���f�[�^�p �萔�o�b�t�@
		std::shared_ptr<KdConstantBuffer>		m_cBuffer;

		// �e�N�X�`��
		std::unordered_map<uint32_t, std::shared_ptr<KdTexture>>	m_textures;
	};

	// �V�F�[�_�[����쐬
	KdMaterial(const std::shared_ptr<KdShader>& shader)
	{
		m_shader = shader;

		// m_shader����p�X���쐬����
		CreatePassesFromShader(nullptr);
	}

	// ���̃}�e���A������쐬
	KdMaterial(KdMaterial& material)
	{
		m_name = material.m_name;
		m_shader = material.m_shader;

		// m_shader����p�X���쐬����
		CreatePassesFromShader(&material);
	}

	// �������쐬���Ԃ�
	std::shared_ptr<KdMaterial> Clone()
	{
		auto mate = std::make_shared<KdMaterial>(*this);
		mate->m_instanced = true;
		return mate;
	}

	// �V�F�[�_�[�擾
	std::shared_ptr<KdShader> GetShader() const { return m_shader; }

	// �}�e���A����
	const std::string& GetName() const { return m_name; }

	// �p�X������AIndex�擾
	int32_t GetPassIndexFromName(const std::string& name) const { return m_shader->GetPassIndexFromName(name); }

	// �f�[�^���Z�b�g
	void SetData(const std::string& name);

	// �s�����H
	bool IsOpaque() const { return true; }

	// �R�s�[�i�H
	bool IsInstanced() const { return m_instanced; }

	// �}�e���A���̒萔�o�b�t�@�ɒl���Z�b�g
	// �EpassIdx �c �p�X�ԍ�
	// �Ename �c �Z�b�g����ϐ���
	// �Edata �c �Z�b�g����f�[�^
	template<class T>
	void SetValue(uint32_t passIdx, const std::string& name, const T& data)
	{
		Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return;
		if (pass->m_cBuffer == nullptr)return;

		pass->m_cBuffer->SetValue(name, data);
	}

	// �}�e���A���̒萔�o�b�t�@����A�l���擾
	// �EpassIdx �c �p�X�ԍ�
	// �Ename �c �Z�b�g����ϐ���
	// �EretValue �c �擾�����f�[�^���i�[����ϐ�
	template<class T>
	void GetValue(uint32_t passIdx, const std::string& name, T& retValue) const
	{
		const Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return;
		if (pass->m_cBuffer == nullptr)return;

		pass->m_cBuffer->GetValue(name, retValue);
	}

	// �}�e���A���f�[�^��V�F�[�_�[���f�o�C�X�փZ�b�g����
	bool SetToDevice(uint32_t passIdx);

	bool IsValidVS(uint32_t passIdx) const
	{
		const Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return false;

		auto shaderPass = m_shader->GetPass(pass->m_passIndex);
		return shaderPass->m_VS != nullptr;
	}
	
	bool IsValidPS(uint32_t passIdx) const
	{
		const Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return false;

		auto shaderPass = m_shader->GetPass(pass->m_passIndex);
		return shaderPass->m_PS != nullptr;
	}

	bool IsValidGS(uint32_t passIdx) const
	{
		const Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return false;

		auto shaderPass = m_shader->GetPass(pass->m_passIndex);
		return shaderPass->m_GS != nullptr;
	}

	// �e�N�X�`�����Z�b�g
	void SetTexture(uint32_t passIdx, uint32_t bindIdx, const std::shared_ptr<KdTexture>& tex)
	{
		Pass* pass = GetPass(passIdx);
		if (pass == nullptr)return;

		pass->m_textures[bindIdx] = tex;
	}

	Pass* GetPass(uint32_t passIdx)
	{
		if (passIdx >= m_passes.size())return nullptr;
		return &m_passes[passIdx];
	}
	const Pass* GetPass(uint32_t passIdx) const
	{
		if (passIdx >= m_passes.size())return nullptr;
		return &m_passes[passIdx];
	}

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const nlohmann::json& jsonData);

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(nlohmann::json& outJson) const;

private:

	// m_shader����p�X���쐬����
	// �EsrcMaterial�F�f�[�g�������p�����}�e���A���Bnullptr�ň����p���Ȃ��B
	void CreatePassesFromShader(KdMaterial* srcMaterial)
	{
		m_passes.clear();

		// �S�V�F�[�_�[�p�X����A�}�e���A���쐬
		for (int i = 0; i < m_shader->GetPasses().size(); i++)
		{
			auto srcPass = m_shader->GetPasses()[i];
			if (srcPass == nullptr)continue;

			Pass pass;
			pass.m_passIndex = i;

			// �V�F�[�_�[�̒萔�o�b�t�@���
			auto cbIndo = srcPass->GetConstantBufferLayout(0);

			// �萔�o�b�t�@�쐬
			if (cbIndo != nullptr)
			{
				pass.m_cBuffer = std::make_shared<KdConstantBuffer>();

				if (srcMaterial == nullptr)
				{
					pass.m_cBuffer->CreateConstantBuffer(*cbIndo);
				}
				else
				{
					pass.m_cBuffer->CreateConstantBuffer(*cbIndo, &srcMaterial->GetPass(i)->m_cBuffer->GetWorkBuffer()[0]);

					// �e�N�X�`��
					pass.m_textures = srcMaterial->GetPass(i)->m_textures;
				}
			}

			m_passes.push_back(pass);
		}
	}

private:

	// �}�e���A����
	std::string						m_name;

	// �V�F�[�_�[
	std::shared_ptr<KdShader>		m_shader;

	// �V�F�[�_�[�p�X�P�ʂ̃f�[�^
	std::vector<Pass>				m_passes;


	// �R�s�[�i���H
	bool							m_instanced = false;

private:
	// �R�s�[�֎~�p
	KdMaterial(const KdMaterial& src) = delete;
	void operator=(const KdMaterial& src) = delete;
};
