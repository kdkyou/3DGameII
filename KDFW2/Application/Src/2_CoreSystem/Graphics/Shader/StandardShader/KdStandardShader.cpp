#include "KdStandardShader.h"


KdStandardShader::KdStandardShader()
{


}

void KdStandardShader::Initialize(const std::string& folderPath, const std::string& hlslFilename)
{
	// �ʏ�`��p
	{
		auto pass = std::make_shared<KdShaderPass>();
		pass->m_name = "Default";

		std::vector<char> compiledBuffer;
		std::string errorMsg;

		// ���_�V�F�[�_
		if (pass->LoadShader(folderPath, hlslFilename, "VS", "vs_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "�V�F�[�_�[�R���p�C���G���[", MB_ICONERROR | MB_OK);
			return;
		}

		// ���_�V�F�[�_�[�쐬
		if (pass->CreateVS(compiledBuffer) == false)return;
		// ���_���̓��C�A�E�g�쐬
		if (pass->CreateInputLayout(compiledBuffer) == false)return;

		// �s�N�Z���V�F�[�_
		if (pass->LoadShader(folderPath, hlslFilename, "PS", "ps_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "�V�F�[�_�[�R���p�C���G���[", MB_ICONERROR | MB_OK);
			return;
		}

		// �s�N�Z���V�F�[�_�[�쐬
		if (pass->CreatePS(compiledBuffer) == false)return;

		//
		SetPass(PassTags::Forward, pass);
	}


	// �V���h�E�L���X�^�[�p
	{
		auto pass = std::make_shared<KdShaderPass>();
		pass->m_name = "ShadowCaster";

		std::vector<char> compiledBuffer;
		std::string errorMsg;

		// ���_�V�F�[�_
		if (pass->LoadShader(folderPath, hlslFilename, "ShadowCasterVS", "vs_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "�V�F�[�_�[�R���p�C���G���[", MB_ICONERROR | MB_OK);
			return;
		}

		// ���_�V�F�[�_�[�쐬
		if (pass->CreateVS(compiledBuffer) == false)return;
		// ���_���̓��C�A�E�g�쐬
		if (pass->CreateInputLayout(compiledBuffer) == false)return;

		// �s�N�Z���V�F�[�_
		if (pass->LoadShader(folderPath, hlslFilename, "ShadowCasterPS", "ps_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "�V�F�[�_�[�R���p�C���G���[", MB_ICONERROR | MB_OK);
			return;
		}

		// �s�N�Z���V�F�[�_�[�쐬
		if (pass->CreatePS(compiledBuffer) == false)return;

		//
		SetPass(PassTags::ShadowCaster, pass);
	}
}

void KdStandardShader::Release()
{
	m_passes.clear();
}
