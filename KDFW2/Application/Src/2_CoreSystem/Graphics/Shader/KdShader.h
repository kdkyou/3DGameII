#pragma once

#include "../KdDirectX.h"

#include "../KdBuffer.h"
#include "../KdTexture.h"

//#include "nlohmann/KdJsonUtility.h"

class KdMaterial;

class KdConstantBufferLayoutInfo;

//============================================
// 
//============================================
class KdResourceInputLayoutInfo
{
public:
	uint32_t		m_bindIdx;
	std::string		m_name;
};

//============================================
// 
// �V�F�[�_�[�p�X
// �E�ȒP�Ɍ����ƁA�P��ނ̃V�F�[�_�[���܂Ƃ߂�����
// �EVS��PS��GS������
// �E�萔�o�b�t�@�̃��C�A�E�g���
// �E�e�N�X�`���Ȃǂ̓��͏��
// 
//============================================
struct KdShaderPass
{
	//=======================================================
	// HLSL�t�@�C�����R���p�C�����A�t�@�C���Ƃ��ĕۑ�����
	// �ۑ����cso�t�@�C����HLSL�t�@�C���̍X�V�������`�F�b�N���AHLSL�X�V���̂݃R���p�C�������
	//=======================================================
	bool LoadShader(
		const std::filesystem::path& baseFolder,	// �x�[�X�ƂȂ�t�H���_
		const std::filesystem::path& hlslFilename,	// HLSL�t�@�C����
		const std::string& entryName,				// �R���p�C������֐���
		const std::string& shaderType,				// �V�F�[�_�[
		std::vector<D3D_SHADER_MACRO> defines,		// Define
		std::vector<char>& outputCompledBuffer,		// (out)�R���p�C���ς݂̃V�F�[�_�[�o�b�t�@
		std::string& errorMsg);						// (out)�G���[���b�Z�[�W

	//=======================================================
	// �R���p�C���ς݃V�F�[�_�[�t�@�C��(cso)��ǂݍ���
	// outputCompledBuffer �c �R���p�C���ς݂̃V�F�[�_�[�o�C�i�����i�[�����
	//=======================================================
	void ReadCSOFile(
		const std::filesystem::path& filename,			// cso�t�@�C����
		std::vector<char>& outputCompledBuffer);		// �R���p�C���ς݂̃V�F�[�_�[�o�b�t�@

	// �萔�o�b�t�@���C�A�E�g���擾
	const KdConstantBufferLayoutInfo* GetConstantBufferLayout(uint32_t slotNo) const
	{
		auto it = m_cbLayouts.find(slotNo);
		if (it == m_cbLayouts.end())return nullptr;
		return &it->second;
	}
	// ���\�[�X���͏��擾�擾
	const KdResourceInputLayoutInfo* GetResourceInputInfo(uint32_t slotNo) const
	{
		auto it = m_resourceInfos.find(slotNo);
		if (it == m_resourceInfos.end())return nullptr;
		return &it->second;
	}

	// �R���p�C���ς݃V�F�[�_�[�o�b�t�@����A���C�A�E�g�����쐬����
	void CreateLayoutFromShader(const std::vector<char>& compiledBuffer);

	// ���_�V�F�[�_�[�쐬
	bool CreateVS(const std::vector<char>& compiledBuffer);
	bool CreateInputLayout(const std::vector<char>& compiledBuffer,
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			1,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		2,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		3,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		4,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		}
	);
	// �W�I���g���V�F�[�_�[�쐬
	bool CreateGS(const std::vector<char>& compiledBuffer);
	// �s�N�Z���V�F�[�_�[�쐬
	bool CreatePS(const std::vector<char>& compiledBuffer);

	// 
	const std::unordered_map<uint32_t, KdResourceInputLayoutInfo>& GetResouceInputInfos() const { return m_resourceInfos; }

	// 
	KdShaderPass() {}

//private:

	std::string						m_name;						// �p�X��
	ComPtr<ID3D11VertexShader>		m_VS = nullptr;				// ���_�V�F�[�_�[
	ComPtr<ID3D11InputLayout>		m_inputLayout = nullptr;	// ���_���̓��C�A�E�g
	ComPtr<ID3D11PixelShader>		m_PS = nullptr;				// �s�N�Z���V�F�[�_�[
	ComPtr<ID3D11GeometryShader>	m_GS = nullptr;				// �W�I���g���V�F�[�_�[

	// �}�e���A���萔�o�b�t�@���C�A�E�g���
	std::unordered_map<uint32_t, KdConstantBufferLayoutInfo>	m_cbLayouts;

	// ���\�[�X���̓��C�A�E�g���
	std::unordered_map<uint32_t, KdResourceInputLayoutInfo>		m_resourceInfos;


	// ���[�h���L���p(�����[�h�Ŏg�p)
	struct LoadInfo
	{
		std::string				folderPath;
		std::string				hlslFilename;
		int32_t					passIndex = -1;
		std::string				passName;
		std::string				vsProcName;
		std::string				psProcName;
		std::string				gsProcName;
	};
	LoadInfo	m_loadInfo = {};

private:
	// �R�s�[�֎~�p
	KdShaderPass(const KdShaderPass& src) = delete;
	void operator=(const KdShaderPass& src) = delete;
};

//============================================
// 
// �V�F�[�_�[��{�N���X
// 
// �E�V�F�[�_�[�́u�p�X�v�Ƃ����P�ʂŃO���[�v�������B
// �E�V�F�[�_�[���̒萔�o�b�t�@(Slot0)���A�}�e���A���̃f�[�^�Ƃ��Ďg�p����B
// 
//============================================
class KdShader : public std::enable_shared_from_this<KdShader>
{
public:

	// �V�F�[�_�[���擾
	const std::string& GetName() const { return m_name; }
	// �V�F�[�_�[���ݒ�
	void SetName(const std::string& name) { m_name = name; }

	// �w��Index�̃p�X���擾
	std::shared_ptr<KdShaderPass> GetPass(int32_t passIdx) const
	{
		if (passIdx < 0 || passIdx >= m_passes.size())return nullptr;
		return m_passes[passIdx];
	}

	// �p�X�̓o�^������Index���擾
	int32_t GetPassIndexFromName(const std::string& name) const
	{
		for (int32_t i = 0; i < (int32_t)m_passes.size(); i++)
		{
			if (m_passes[i]->m_name == name)return i;
		}
		return -1;
	}

	// �S�p�X�̃��X�g���擾
	const std::vector<std::shared_ptr<KdShaderPass>>& GetPasses() const { return m_passes; }

	// �w��Index�ɃV�F�[�_�[�p�X��o�^
	void SetPass(uint32_t passIndex, const std::shared_ptr<KdShaderPass>& pass)
	{
		if (passIndex >= (uint32_t)m_passes.size())
		{
			m_passes.resize(passIndex + 1);
		}

		m_passes[passIndex] = pass;
	}

	// �V�F�[�_�[���쐬���A�w��Index�̃p�X�Ƃ��Ď擾����
	// �EvsProcName		�F���_�V�F�[�_�[�̊֐������w��
	// �EpsProcName		�F�s�N�Z���V�F�[�_�[�̊֐������w��
	// �EgsProcName		�F�W�I���g���V�F�[�_�[�̊֐������w��
	void LoadShaderAndSetPass(
		const std::string& folderPath, const std::string& hlslFilename,
		uint32_t passIndex, const std::string& passName,
		const std::string& vsProcName, const std::string& psProcName, const std::string& gsProcName = "");

	// 
	void ReloadShader();

	// �}�e���A���쐬
	std::shared_ptr<KdMaterial> CreateMaterial()
	{
		return std::make_shared<KdMaterial>(shared_from_this());
	}

	KdShader() {}

	// 
	~KdShader()
	{
		m_passes.clear();
	}

	// 
	enum PassTags : uint32_t
	{
		Forward = 0,			// �ʏ�`��p�p�X
		ShadowCaster = 1,		// �V���h�E�}�b�v�`��p
		DepthOnly = 2,			// �[�x�`��̂�

		// ���R�ɒǉ��E�ύX���Ă�������
	};

protected:

	// �V�F�[�_�[��
	std::string m_name;

	// �V�F�[�_�[�p�X���X�g
	std::vector<std::shared_ptr<KdShaderPass>>			m_passes;

private:
	// �R�s�[�֎~�p
	KdShader(const KdShader& src) = delete;
	void operator=(const KdShader& src) = delete;
};
