#include "PP_ToneMapping.h"

void PP_ToneMapping::Initialize()
{
	m_shader = std::make_shared<KdShader>();

	// シェーダー
	m_shader->LoadShaderAndSetPass("Assets/Shader/PP_ToneMapping", "PP_ToneMapping.hlsl",
		0, "ToneMapping",
		"", "ACESFilmicPS", "");

	// マテリアル作成
	m_material = m_shader->CreateMaterial();
}

void PP_ToneMapping::Execute(KdScreenData& screenData)
{
	if (m_enable == false)return;

	KdShaderManager::GetInstance().m_blitShader.Draw(screenData.GetInputColorTex().get(), screenData.GetColorTex().get(), m_material.get());

	// カラーバッファを入力用のバッファにコピー
	screenData.CopyColorTex();
}
