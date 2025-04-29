#include "KdShader.h"

static const char* g_includePath = "Src\\2_CoreSystem\\Graphics\\Shader";

// file1��file2�̍X�V�������`�F�b�N���Afile1�̕����V�����ꍇ��true
static bool CompareFileTime(const std::filesystem::path& file1, const std::filesystem::path& file2)
{
	std::filesystem::file_time_type time1 = std::filesystem::last_write_time(file1);
	std::filesystem::file_time_type time2 = std::filesystem::last_write_time(file2);

	if (time1 > time2)return true;

	return false;
}

//========================================================
// 
// HLSL�\�[�X��͎���#include���������邽�߂̃N���X
// 
//========================================================
class ShaderIncludeClass : public ID3DInclude
{
public:
	// �C���N���[�h�t�@�C���̃f�B���N�g�����X�g
	// #include�́A�����ɓo�^����Ă���t�H���_�Ō��������悤�ɂ��邽�߁A���O�Ƀp�X�����Ă�������
	std::vector<std::filesystem::path> m_dirs;

	// ��͌�A�C���N���[�h�����w�b�_�[�t�@�C���̃p�X���X�g
	std::vector<std::filesystem::path> m_headerFilePathList;

	// �ǂݍ��񂾃w�b�_�[�t�@�C���̓��e
	std::string m_buffer;

	// �w�b�_�[�t�@�C���̃p�X��������
	HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override
	{
		for (auto&& dir : m_dirs)
		{
			// �w�肵���w�b�_�[�t�@�C�����A���̃t�H���_�ɑ��݂���H
			std::filesystem::path headerFilePath = dir / pFileName;

			std::ifstream file(headerFilePath);
			if (file)
			{
				// ���݂���ꍇ�A���̃p�X���L�����Ă���
				m_headerFilePathList.push_back(headerFilePath);

				// �w�b�_�[�t�@�C���̒��g���\�[�X�ɒǉ����邽�߁A�t�@�C����ǂݍ���
				m_buffer = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());	// �t�@�C���ǂݍ���

				// ���̃w�b�_�[�t�@�C���̓��e���AHLSL�\�[�X�R�[�h�ɒǉ������悤�Ɏw��
				*ppData = &m_buffer[0];
				*pBytes = (uint32_t)m_buffer.size();

				return S_OK;
			}
		}

		return S_FALSE;
	}
	HRESULT Close(LPCVOID pData) override
	{
		m_buffer = "";
		return S_OK;
	}
};

// HLSL�t�@�C�����R���p�C�����A�t�@�C���Ƃ��ĕۑ�����
bool KdShaderPass::LoadShader(
	const std::filesystem::path& baseFolder,
	const std::filesystem::path& hlslFilename,
	const std::string& entryName,
	const std::string& shaderType,
	std::vector<D3D_SHADER_MACRO> defines,
	std::vector<char>& outputCompledBuffer,
	std::string& errorMsg)
{
	// define�֌W
	// �I�[��ǉ�
	defines.push_back({ nullptr, nullptr });

	// �t�@�C���p�X
	std::filesystem::path HLSLfullpath = baseFolder / hlslFilename;

	// cso�t�@�C���̃p�X(�R���p�C���ς݃V�F�[�_�[)
	std::string outpuFullFilename = (baseFolder / hlslFilename.stem()).generic_string() + "_" + entryName + ".cso";

	bool existCSOFile = std::filesystem::exists(outpuFullFilename);
	bool existHLSLFile = std::filesystem::exists(HLSLfullpath);
	// �t�@�C�������݂��Ȃ�
	if (existCSOFile == false && existHLSLFile == false)
	{
		outputCompledBuffer.clear();
		errorMsg = "�t�@�C�������݂��Ȃ�";
		return false;
	}

	// CSO�̂ݑ��݂̏ꍇ�́A�����ǂݍ���ŏI��
	if (existCSOFile && existHLSLFile == false)
	{
		ReadCSOFile(outpuFullFilename, outputCompledBuffer);

		errorMsg = "";
		return true;
	}

	//����������������������������������
	// 
	// HLSL�t�@�C���O�������s
	// 
	// ���O�����Ƃ́H
	// �EHLSL�t�@�C������͂��A�\�[�X���ŋL�q����Ă���#include�Ȃǂ̏����擾�B
	// �E#define��#ifdef�ŁA�s�v�ȕ���������΍폜����B
	// �EHLSL����#include��S�Č������A�P�̃\�[�X�R�[�h�ɂ���B
	// 
	//����������������������������������

	// �O�����ς݂�HLSL�\�[�X�R�[�h���i�[����ꏊ
	ComPtr<ID3DBlob> preprocessBlob = nullptr;

	{
		// �ǉ��̃w�b�_�[�t�H���_�p�X
		ShaderIncludeClass inc;
		inc.m_dirs.push_back(baseFolder);
		inc.m_dirs.push_back(g_includePath);	// ���ʂ̃t�@�C�����u���Ă���t�H���_���Ώۂɂ���

		// HLSL�\�[�X�t�@�C�������̂܂ܓǂݍ���
		std::ifstream file(HLSLfullpath.generic_wstring().c_str(), std::ios::binary);
		auto hlslFileData = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		// �ǂݍ���HLSL�\�[�X�t�@�C����O��������
		// (define��include���l�����A�P�̃\�[�X�t�@�C���Ɍ�������)
		ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3DPreprocess(&hlslFileData[0], hlslFileData.size(), nullptr, &defines[0], &inc, &preprocessBlob, &errorBlob);
		if (FAILED(hr))
		{
			// ���s��
			if (errorBlob)
			{
				errorMsg = (char*)errorBlob->GetBufferPointer();
			}

			return false;
		}

		// HLSL��CSO �������݂���ꍇ�́A�X�V�������`�F�b�N
		if (existCSOFile && existHLSLFile)
		{
			//---------------------------------------------------
			// CSO�t�@�C���Ɣ�ׂāA
			// HLSL�t�@�C����e�w�b�_�[�t�@�C���̂ǂꂩ�ɍX�V�����������H
			//---------------------------------------------------

			bool neesCompile = false;

			// include���ꂽ�S�w�b�_�[�t�@�C��
			for (auto&& headerFilePath : inc.m_headerFilePathList)
			{
				// �w�b�_�[�t�@�C���ƁACSO�t�@�C���̍X�V�����`�F�b�N
				// �w�b�_�[�̕����V�����Ȃ�A�ăR���p�C�����K�v�I
				if (CompareFileTime(headerFilePath, outpuFullFilename)) neesCompile = true;
			}

			// HLSL�\�[�X�t�@�C���ƁACSO�t�@�C���̍X�V�����`�F�b�N
			// HLSL�\�[�X�t�@�C���̕����V�����Ȃ�A�ăR���p�C�����K�v�I
			if (CompareFileTime(HLSLfullpath, outpuFullFilename)) neesCompile = true;

			// �ăR���p�C�����K�v�Ȃ��Ȃ�ACSO�����̂܂ܓǂݍ���ŏI��
			if (neesCompile == false)
			{
				ReadCSOFile(outpuFullFilename, outputCompledBuffer);

				errorMsg = "";
				return true;
			}
		}
	}


	//����������������������������������
	// 
	// HLSL�V�F�[�_�[�R�[�h�̃R���p�C��
	// 
	//����������������������������������

	{
		// �R���p�C���t���O
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;// | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#ifdef _DEBUG
		flags |= D3DCOMPILE_DEBUG;
#endif

		// �R���p�C�����ꂽ�V�F�[�_�[�o�C�i�����i�[����ꏊ
		ComPtr<ID3DBlob> shaderBlob = nullptr;
		ComPtr<ID3DBlob> errorBlob = nullptr;

		// �O�������ꂽ�V�F�[�_�[�R�[�h���R���p�C��
		HRESULT hr = D3DCompile(preprocessBlob->GetBufferPointer(), preprocessBlob->GetBufferSize(), nullptr, nullptr, nullptr,
			entryName.c_str(), shaderType.c_str(),
			flags, 0, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			// ���s��
			if (errorBlob)
			{
				errorMsg = (char*)errorBlob->GetBufferPointer();
			}

			return false;
		}

		// �R���p�C�����ꂽ�V�F�[�_�[�o�C�i����Ԃ�(�R�s�[����)
		outputCompledBuffer.resize(shaderBlob->GetBufferSize());
		memcpy_s(outputCompledBuffer.data(), shaderBlob->GetBufferSize(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
	}

	//����������������������������������
	// 
	// �V�F�[�_�[���e�̉��
	// 
	// �E�萔�o�b�t�@�̏���A�e�N�X�`�����͂̏��ȂǁA�V�F�[�_�[���̏ڍׂ�m�邱�Ƃ��o����
	// 
	//����������������������������������
	if (outputCompledBuffer.size() > 0)
	{
		// �R���p�C���ς݃V�F�[�_�[����A�ڍ׏����擾����
		CreateLayoutFromShader(outputCompledBuffer);
	}

	//����������������������������������
	// 
	// CSO�t�@�C���t�@�C���ۑ�
	// 
	//����������������������������������
	// �R���p�C���ς݃V�F�[�_�[���Acso�t�@�C���t�@�C���ɕۑ�����
	std::ofstream ofs(outpuFullFilename, std::ios::binary);
	if (ofs)
	{
		ofs.write(&outputCompledBuffer[0], outputCompledBuffer.size());
	}

	errorMsg = "";
	return true;
}

void KdShaderPass::ReadCSOFile(const std::filesystem::path& filename, std::vector<char>& outputCompledBuffer)
{
	outputCompledBuffer.clear();

	//------------------------
	// �R���p�C���ς݃V�F�[�_�[�t�@�C���̓ǂݍ���
	//------------------------
	std::ifstream file(filename, std::ios::binary);
	if (!file) return;

	// �t�@�C���ǂݍ���
	outputCompledBuffer = std::vector(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

	//------------------------
	// �R���p�C���ς݃V�F�[�_�[����A
	// �ڍ׏����擾����
	//------------------------
	if (outputCompledBuffer.size() > 0)
	{
		CreateLayoutFromShader(outputCompledBuffer);
	}
}


void KdShaderPass::CreateLayoutFromShader(const std::vector<char>& compiledBuffer)
{
	//==========================================
	// 
	// �R���p�C���ς݃V�F�[�_�[����A�ڍ׏����擾����
	// 
	//==========================================
	ComPtr<ID3D11ShaderReflection> reflection;
	if (SUCCEEDED(D3DReflect(&compiledBuffer[0], compiledBuffer.size(), IID_ID3D11ShaderReflection, (void**)&reflection)))
	{
		// �V�F�[�_�[�̏��
		D3D11_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);

		// ���\�[�X�̃o�C���h���
		for (uint32_t iBind = 0; iBind < shaderDesc.BoundResources; iBind++)
		{
			// �o�C���h���
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			reflection->GetResourceBindingDesc(iBind, &bindDesc);

			// �萔�o�b�t�@����0�ԖڂɃo�C���h����Ă��郄�c
			if (bindDesc.Type == D3D_SIT_CBUFFER)
			{
				KdConstantBufferLayoutInfo cbInfo;

				// �萔�o�b�t�@��������擾
				auto pCB = reflection->GetConstantBufferByName(bindDesc.Name);
				D3D11_SHADER_BUFFER_DESC cbDesc;
				pCB->GetDesc(&cbDesc);

				// �萔�o�b�t�@�̃T�C�Y���L�����Ă���
				cbInfo.SetBufferSize(cbDesc.Size);

				// �萔�o�b�t�@���̏��
				for (uint32_t i = 0; i < cbDesc.Variables; i++)
				{
					auto pVariable = pCB->GetVariableByIndex(i);
					D3D11_SHADER_VARIABLE_DESC variDesc;
					pVariable->GetDesc(&variDesc);

					// �^
					D3D11_SHADER_TYPE_DESC typeDesc;
					pVariable->GetType()->GetDesc(&typeDesc);

					// �����o����
					KdConstantBufferLayoutInfo::VariableData data;
					data.Name = variDesc.Name;
					data.StartOffset = variDesc.StartOffset;
					data.ByteSize = variDesc.Size;

					// �^
					switch (typeDesc.Type)
					{
					case D3D_SHADER_VARIABLE_TYPE::D3D10_SVT_FLOAT:
						data.Type = KdConstantBufferLayoutInfo::Float;
						break;
					case D3D_SHADER_VARIABLE_TYPE::D3D10_SVT_INT:
						data.Type = KdConstantBufferLayoutInfo::Int;
						break;
					default:
						data.Type = KdConstantBufferLayoutInfo::Float;
						break;
					};

					// �s���ƌ���
					data.Rows = typeDesc.Rows;
					data.Columns = typeDesc.Columns;

					cbInfo.SetVariableData(data);
				}

				// �o�^
				m_cbLayouts[bindDesc.BindPoint] = cbInfo;
			}
			else if (bindDesc.Type == D3D_SIT_TEXTURE)
			{
				D3D11_SHADER_INPUT_BIND_DESC sibDesc;
				reflection->GetResourceBindingDescByName(bindDesc.Name, &sibDesc);

				KdResourceInputLayoutInfo info = {};
				info.m_bindIdx = sibDesc.BindPoint;
				info.m_name = sibDesc.Name;

				// �o�^
				m_resourceInfos[sibDesc.BindPoint] = info;
			}
		}
	}
}

bool KdShaderPass::CreateVS(const std::vector<char>& compiledBuffer)
{
	// �V�F�[�_�[�쐬
	if (FAILED(D3D.GetDev()->CreateVertexShader(&compiledBuffer[0], compiledBuffer.size(), nullptr, &m_VS)))
	{
		assert(0 && "���_�V�F�[�_�[�쐬���s");
		return false;
	}
	return true;
}

bool KdShaderPass::CreateInputLayout(const std::vector<char>& compiledBuffer, std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout)
{
	// �P���_�̏ڍׂȏ��
	/*
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			1,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,		2,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		3,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,		4,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	*/

	// ���_���̓��C�A�E�g�쐬
	if (FAILED(D3D.GetDev()->CreateInputLayout(
		&inputLayout[0],								// ���̓G�������g�擪�A�h���X
		(uint32_t)inputLayout.size(),				// ���̓G�������g��
		&compiledBuffer[0],						// ���_�o�b�t�@�̃o�C�i���f�[�^
		(uint32_t)compiledBuffer.size(),		// ��L�̃o�b�t�@�T�C�Y
		&m_inputLayout))					// 
		)
	{
		assert(0 && "CreateInputLayout�쐬���s");
		return false;
	}

	return true;
}

bool KdShaderPass::CreateGS(const std::vector<char>& compiledBuffer)
{
	// �V�F�[�_�[�쐬
	if (FAILED(D3D.GetDev()->CreateGeometryShader(&compiledBuffer[0], compiledBuffer.size(), nullptr, &m_GS)))
	{
		assert(0 && "�W�I���g���V�F�[�_�[�쐬���s");
		return false;
	}
	return true;
}

bool KdShaderPass::CreatePS(const std::vector<char>& compiledBuffer)
{
	// �V�F�[�_�[�쐬
	if (FAILED(D3D.GetDev()->CreatePixelShader(&compiledBuffer[0], compiledBuffer.size(), nullptr, &m_PS)))
	{
		assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
		return false;
	}
	return true;
}

void KdShader::LoadShaderAndSetPass(const std::string& folderPath, const std::string& hlslFilename,
	uint32_t passIndex, const std::string& passName,
	const std::string& vsProcName, const std::string& psProcName, const std::string& gsProcName)
{
	auto pass = std::make_shared<KdShaderPass>();
	pass->m_name = passName;

	std::vector<char> compiledBuffer;
	std::string errorMsg;

	// ���_�V�F�[�_
	if (vsProcName.empty() == false)
	{
		if (pass->LoadShader(folderPath, hlslFilename, vsProcName.c_str(), "vs_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "�V�F�[�_�[�R���p�C���G���[", MB_ICONERROR | MB_OK);
			return;
		}

		// ���_�V�F�[�_�[�쐬
		if (pass->CreateVS(compiledBuffer) == false)return;
		// ���_���̓��C�A�E�g�쐬
		if (pass->CreateInputLayout(compiledBuffer) == false)return;
	}

	// �s�N�Z���V�F�[�_
	if (psProcName.empty() == false)
	{
		if (pass->LoadShader(folderPath, hlslFilename, psProcName.c_str(), "ps_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "�V�F�[�_�[�R���p�C���G���[", MB_ICONERROR | MB_OK);
			return;
		}

		// �s�N�Z���V�F�[�_�[�쐬
		if (pass->CreatePS(compiledBuffer) == false)return;
	}

	// �W�I���g���V�F�[�_
	if (gsProcName.empty() == false)
	{
		if (pass->LoadShader(folderPath, hlslFilename, gsProcName.c_str(), "gs_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "�V�F�[�_�[�R���p�C���G���[", MB_ICONERROR | MB_OK);
			return;
		}

		// �W�I���g���V�F�[�_�[�쐬
		if (pass->CreateGS(compiledBuffer) == false)return;
	}

	// ���[�h�����L�����Ă���
	pass->m_loadInfo.folderPath = folderPath;
	pass->m_loadInfo.hlslFilename = hlslFilename;
	pass->m_loadInfo.passIndex = passIndex;
	pass->m_loadInfo.passName = passName;
	pass->m_loadInfo.vsProcName = vsProcName;
	pass->m_loadInfo.psProcName = psProcName;
	pass->m_loadInfo.gsProcName = gsProcName;

	// �p�X��ǉ�
	SetPass(passIndex, pass);


}

void KdShader::ReloadShader()
{
	for (auto&& pass : m_passes)
	{
		LoadShaderAndSetPass(
			pass->m_loadInfo.folderPath,
			pass->m_loadInfo.hlslFilename,
			pass->m_loadInfo.passIndex,
			pass->m_loadInfo.passName,
			pass->m_loadInfo.vsProcName,
			pass->m_loadInfo.psProcName,
			pass->m_loadInfo.gsProcName
		);
	}

}
