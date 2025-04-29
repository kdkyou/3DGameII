#include "PP_SSAO.h"

void PP_SSAO::Initialize()
{
	m_shader = std::make_shared<KdShader>();


	// SSAO
	m_shader->LoadShaderAndSetPass("Assets/Shader/PP_SSAO", "PP_SSAO.hlsl",
		kPass_SSAO, "SSAO",
		"", "SSAOPS", "");

	// BilateralBlur
	m_shader->LoadShaderAndSetPass("Assets/Shader/PP_SSAO", "PP_SSAO.hlsl",
		kPass_BilateralBlur, "BilateralBlur",
		"", "BilateralBlurPS", "");

	// マテリアル作成
	m_material = m_shader->CreateMaterial();

	// SSAO
//	m_material->SetValue<int>(kPass_SSAO, "g_SampleCount", 9);
//	m_material->SetValue<float>(kPass_SSAO, "g_Radius", 0.3f);
//	m_material->SetValue<float>(kPass_SSAO, "g_Sigma", 0.1f);
//	m_material->SetValue<float>(kPass_SSAO, "g_Beta", 0.001f);
//	m_material->SetValue<float>(kPass_SSAO, "g_Epsilon", 0.001f);

	// Bilateral Blur
//	m_material->SetValue<int>(kPass_BilateralBlur, "g_bb_SampleCount", 21);
//	m_material->SetValue<float>(kPass_BilateralBlur, "g_lumin", 2.0f);
//	m_material->SetValue<float>(kPass_BilateralBlur, "g_dispersion", 2.0);

	// 作業用テクスチャ作成
	m_rt[0] = std::make_shared<KdRenderTexture>();
	m_rt[0]->CreateRenderTexture(D3D.GetWidth() / 4, D3D.GetHeight() / 4, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, nullptr, DXGI_FORMAT_UNKNOWN);

	m_rt[1] = std::make_shared<KdRenderTexture>();
	m_rt[1]->CreateRenderTexture(D3D.GetWidth() / 4, D3D.GetHeight() / 4, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, nullptr, DXGI_FORMAT_UNKNOWN);

}

void PP_SSAO::Execute(KdScreenData& screenData)
{
	if (m_enable == false)return;

	// SSAO
	m_material->SetValue<int>(kPass_SSAO, "g_SampleCount", m_sampleCount);
	m_material->SetValue<float>(kPass_SSAO, "g_Radius", m_radius);
	m_material->SetValue<float>(kPass_SSAO, "g_Sigma", m_sigma);
	m_material->SetValue<float>(kPass_SSAO, "g_Beta", m_beta);
	m_material->SetValue<float>(kPass_SSAO, "g_Epsilon", m_epsilon);
	KdShaderManager::GetInstance().m_blitShader.Draw(nullptr, m_rt[0].get(), m_material.get(), kPass_SSAO);


	m_material->SetValue<int>(kPass_BilateralBlur, "g_bb_SampleCount", m_sampleCount);
	m_material->SetValue<float>(kPass_BilateralBlur, "g_lumin", m_lumin);
	m_material->SetValue<float>(kPass_BilateralBlur, "g_dispersion", m_dispersion);

	// Xブラー
	for (int i = 0; i < 5; i++)
	{
		m_material->SetValue<KdVector2>(1, "g_direction", { 1.0f / m_rt[0]->GetWidth(), 0 });
		KdShaderManager::GetInstance().m_blitShader.Draw(m_rt[0].get(), m_rt[1].get(), m_material.get(), kPass_BilateralBlur);

		// Yブラー
		m_material->SetValue<KdVector2>(1, "g_direction", { 0, 1.0f / m_rt[0]->GetHeight() });
		KdShaderManager::GetInstance().m_blitShader.Draw(m_rt[1].get(), m_rt[0].get(), m_material.get(), kPass_BilateralBlur);
	}

	if (m_DEBUG_ShowSSAOOnly == false)
	{
		// 結果を乗算合成
		KdShaderManager::GetInstance().m_bs_Multiply->SetToDevice();

		KdShaderManager::GetInstance().m_blitShader.Draw(m_rt[0].get(), screenData.GetColorTex().get());
	}
	// デバッグ
	else
	{
		KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();
		KdShaderManager::GetInstance().m_blitShader.Draw(m_rt[0].get(), screenData.GetColorTex().get());
	}

	KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();

	// カラーバッファを入力用のバッファにコピー
	screenData.CopyColorTex();
}

void PP_SSAO::Editor_ImGui()
{
	KdPostProcessBase::Editor_ImGui();

	ImGui::DragInt(u8"Sample Count", &m_sampleCount);

	ImGui::DragFloat(u8"Radius", &m_radius, 0.01f);
	ImGui::DragFloat(u8"Sigma", &m_sigma, 0.001f);
	ImGui::DragFloat(u8"Beta", &m_beta, 0.001f);
	ImGui::DragFloat(u8"Epsilon", &m_epsilon, 0.001f);

	ImGui::DragFloat(u8"Lumin", &m_lumin, 0.001f);
	ImGui::DragFloat(u8"Dispersion", &m_dispersion, 0.01f);

	ImGui::Checkbox(u8"(デバッグ)SSAOのみ表示", &m_DEBUG_ShowSSAOOnly);
}
