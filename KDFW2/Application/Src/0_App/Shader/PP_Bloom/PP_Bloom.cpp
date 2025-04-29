#include "PP_Bloom.h"

void PP_Bloom::Initialize()
{
	m_shader = std::make_shared<KdShader>();


	// DownSampling2x2
	m_shader->LoadShaderAndSetPass("Assets/Shader/PP_Bloom", "PP_Bloom.hlsl",
		kPass_DownSampling2x2, "DownSampling2x2",
		"", "DownSampling2x2PS", "");

	// BrightFiltering
	m_shader->LoadShaderAndSetPass("Assets/Shader/PP_Bloom", "PP_Bloom.hlsl",
		kPass_BrightFiltering, "BrightFiltering",
		"", "BrightFilteringPS", "");

	// Blur
	m_shader->LoadShaderAndSetPass("Assets/Shader/PP_Bloom", "PP_Bloom.hlsl",
		kPass_Blur, "Blur",
		"", "BlurPS", "");

	// マテリアル作成
	m_material = m_shader->CreateMaterial();

	// 作業用テクスチャ作成

	m_highBrightness = std::make_shared<KdRenderTexture>();
	m_highBrightness->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, nullptr, DXGI_FORMAT_UNKNOWN);

	int divideVal = 2;
	for (int i = 0; i < kBlurCount; i++)
	{
		m_rt[i][0] = std::make_shared<KdRenderTexture>();
		m_rt[i][0]->CreateRenderTexture(D3D.GetWidth() / divideVal, D3D.GetHeight() / divideVal, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, nullptr, DXGI_FORMAT_UNKNOWN);

		m_rt[i][1] = std::make_shared<KdRenderTexture>();
		m_rt[i][1]->CreateRenderTexture(D3D.GetWidth() / divideVal, D3D.GetHeight() / divideVal, DXGI_FORMAT_R16G16B16A16_FLOAT, 1, nullptr, DXGI_FORMAT_UNKNOWN);

		divideVal *= 2;
	}

}

void PP_Bloom::Execute(KdScreenData& screenData)
{
	if (m_enable == false)return;

//	KdShaderManager::GetInstance().m_blitShader.Draw(screenData.GetInputColorTex().get(), screenData.GetColorTex().get(), m_material.get());

	KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();

	// 高輝度抽出
	m_material->SetValue(kPass_BrightFiltering, "g_BrightThreshold", m_brightThreshold);
	KdShaderManager::GetInstance().m_blitShader.Draw(screenData.GetInputColorTex().get(), m_highBrightness.get(), m_material.get(), kPass_BrightFiltering);

//	KdShaderManager::GetInstance().m_blitShader.Draw(m_highBrightness.get(), screenData.GetColorTex().get());
//	screenData.CopyColorTex();
//	return;

	m_material->SetValue<int>(kPass_Blur, "g_sampleCount", m_sampleCount);
	m_material->SetValue<float>(kPass_Blur, "g_dispersion", m_dispersion);

	for (int i = 0; i < kBlurCount; i++)
	{
		// 縮小コピー
		KdRenderTexture* srcTex = nullptr;
		if (i == 0)
		{
			srcTex = m_highBrightness.get();
		}
		else
		{
			srcTex = m_rt[i - 1][0].get();
		}

		/*
		KdVector2 texelSize(1.0f / srcTex->GetWidth(), 1.0f / srcTex->GetHeight());
		KdVector4 sampleOffsets[4] = {};
		KdVector4* ptr = sampleOffsets;
		for (int y = 0; y < 2; ++y)
		{
			for (int x = 0; x < 2; ++x)
			{
				ptr->x = (x - 0.5f) * texelSize.x;
				ptr->y = (y - 0.5f) * texelSize.y;
				++ptr;
			}
		}
		m_material->SetValue(kPass_DownSampling2x2, "g_sampleOffsets", sampleOffsets);

		KdShaderManager::GetInstance().m_blitShader.Draw(srcTex, m_rt[i][0].get(), m_material.get(), kPass_DownSampling2x2);
		*/
		KdShaderManager::GetInstance().m_blitShader.Draw(srcTex, m_rt[i][0].get());

		// Xブラー
		m_material->SetValue<KdVector2>(kPass_Blur, "g_direction", { 1.0f / m_rt[i][0]->GetWidth(), 0 });
		KdShaderManager::GetInstance().m_blitShader.Draw(m_rt[i][0].get(), m_rt[i][1].get(), m_material.get(), kPass_Blur);

		// Yブラー
		m_material->SetValue<KdVector2>(kPass_Blur, "g_direction", { 0, 1.0f / m_rt[i][0]->GetHeight() });
		KdShaderManager::GetInstance().m_blitShader.Draw(m_rt[i][1].get(), m_rt[i][0].get(), m_material.get(), kPass_Blur);
	}


	if (m_DEBUG_ShowBlurOnly == false)
	{
		KdShaderManager::GetInstance().m_bs_Add->SetToDevice();

		// 結果を乗算合成
		for (int i = 0; i < kBlurCount; i++)
		{
			KdShaderManager::GetInstance().m_blitShader.Draw(m_rt[i][0].get(), screenData.GetColorTex().get());
		}
	}
	// デバッグ表示
	else
	{
		for (int i = 0; i < kBlurCount; i++)
		{
			if(i == 0)KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();
			else KdShaderManager::GetInstance().m_bs_Add->SetToDevice();

			KdShaderManager::GetInstance().m_blitShader.Draw(m_rt[i][0].get(), screenData.GetColorTex().get());
		}
	}

	// 
	KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();

	// カラーバッファを入力用のバッファにコピー
	screenData.CopyColorTex();
}

void PP_Bloom::Editor_ImGui()
{
	KdPostProcessBase::Editor_ImGui();

	ImGui::DragFloat("高輝度抽出の閾値", &m_brightThreshold, 0.01f);

	ImGui::DragInt("m_sampleCount", &m_sampleCount);
	ImGui::DragFloat("g_dispersion", &m_dispersion, 0.01f);

	ImGui::Checkbox(u8"(デバッグ)ぼかしのみ表示", &m_DEBUG_ShowBlurOnly);
}
