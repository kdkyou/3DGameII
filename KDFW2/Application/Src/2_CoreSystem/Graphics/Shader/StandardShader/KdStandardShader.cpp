#include "KdStandardShader.h"


KdStandardShader::KdStandardShader()
{


}

void KdStandardShader::Initialize(const std::string& folderPath, const std::string& hlslFilename)
{
	// 通常描画用
	{
		auto pass = std::make_shared<KdShaderPass>();
		pass->m_name = "Default";

		std::vector<char> compiledBuffer;
		std::string errorMsg;

		// 頂点シェーダ
		if (pass->LoadShader(folderPath, hlslFilename, "VS", "vs_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "シェーダーコンパイルエラー", MB_ICONERROR | MB_OK);
			return;
		}

		// 頂点シェーダー作成
		if (pass->CreateVS(compiledBuffer) == false)return;
		// 頂点入力レイアウト作成
		if (pass->CreateInputLayout(compiledBuffer) == false)return;

		// ピクセルシェーダ
		if (pass->LoadShader(folderPath, hlslFilename, "PS", "ps_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "シェーダーコンパイルエラー", MB_ICONERROR | MB_OK);
			return;
		}

		// ピクセルシェーダー作成
		if (pass->CreatePS(compiledBuffer) == false)return;

		//
		SetPass(PassTags::Forward, pass);
	}


	// シャドウキャスター用
	{
		auto pass = std::make_shared<KdShaderPass>();
		pass->m_name = "ShadowCaster";

		std::vector<char> compiledBuffer;
		std::string errorMsg;

		// 頂点シェーダ
		if (pass->LoadShader(folderPath, hlslFilename, "ShadowCasterVS", "vs_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "シェーダーコンパイルエラー", MB_ICONERROR | MB_OK);
			return;
		}

		// 頂点シェーダー作成
		if (pass->CreateVS(compiledBuffer) == false)return;
		// 頂点入力レイアウト作成
		if (pass->CreateInputLayout(compiledBuffer) == false)return;

		// ピクセルシェーダ
		if (pass->LoadShader(folderPath, hlslFilename, "ShadowCasterPS", "ps_5_0", {}, compiledBuffer, errorMsg) == false)
		{
			MessageBoxA(NULL, errorMsg.c_str(), "シェーダーコンパイルエラー", MB_ICONERROR | MB_OK);
			return;
		}

		// ピクセルシェーダー作成
		if (pass->CreatePS(compiledBuffer) == false)return;

		//
		SetPass(PassTags::ShadowCaster, pass);
	}
}

void KdStandardShader::Release()
{
	m_passes.clear();
}
