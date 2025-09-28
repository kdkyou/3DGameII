#include "KdFramework.h"

#include "PP_ColocCollection.h"

void PP_ColorCollection::Initialize()
{
	m_shader = std::make_shared<KdShader>();
	m_shader->LoadShaderAndSetPass(
		"Assets/Shader/PP_ColorCollection", // シェーダーファイルのパス
	    "PP_ColorCollection.hlsl", //シェーダーファイル名
		GrayScale, "GrayScale", // パス情報
		"",
		"GrayScale", // どのシェーダーを使うか
		"" //どのジオメトリシェーダーを使うか
	);

	m_shader->LoadShaderAndSetPass(
		"Assets/Shader/PP_ColorCollection",
		"PP_ColorCollection.hlsl",
		Sepia, "Sepia","","Sepia",""
	);
	
	m_shader->LoadShaderAndSetPass(
		"Assets/Shader/PP_ColorCollection",
		"PP_ColorCollection.hlsl",
		TwoGradiation, "TwoGradiation","","TwoGradiation",""
	);

	m_shader->LoadShaderAndSetPass(
		"Assets/Shader/PP_ColorCollection",
		"PP_ColorCollection.hlsl",
		Vignette, "Vignette","","vignette",""
	);

	m_vignetteMask = KdResourceManager::GetInstance().LoadAsset<KdTexture>("Assets/Textures/vignette.png");



	// マテリアルの作成
	m_material = m_shader->CreateMaterial();
}

void PP_ColorCollection::Execute(KdScreenData& screenData)
{
	if (m_enable == false) { return; }
	if (m_isGrayscale)
	{
		KdShaderManager::GetInstance().m_blitShader.Draw(
			screenData.GetInputColorTex().get(),
			screenData.GetColorTex().get(),
			m_material.get(), GrayScale);
		// 現在の出力画像を次の入力画像にする
		screenData.CopyColorTex();

		if (m_isSepia)
		{
			// セピア変換
			KdShaderManager::GetInstance().m_blitShader.Draw(
				screenData.GetInputColorTex().get(),
				screenData.GetColorTex().get(),
				m_material.get(), Sepia);
			screenData.CopyColorTex();
		}
	
		if (m_isTwoGradiation)
		{
			// しきい値の転送
			m_material->SetValue<float>(TwoGradiation,
				"g_twThreshold",
				m_twoGradiationValue);

			// グラディエーション変換
			KdShaderManager::GetInstance().m_blitShader.Draw(
				screenData.GetInputColorTex().get(),
				screenData.GetColorTex().get(),
				m_material.get(), TwoGradiation);
			screenData.CopyColorTex();
		}
	}

	if (m_isVignette)
	{
		m_material->SetTexture(Vignette, 1, m_vignetteMask);
		m_material->SetValue<float>(Vignette, "g_vignetteIntenPow", m_vignetteIntenPow);
		m_material->SetValue<float>(Vignette, "g_vignetteMaskPow", m_vignetteMaskPow);


		// ビネット変換
		KdShaderManager::GetInstance().m_blitShader.Draw(
			screenData.GetInputColorTex().get(),
			screenData.GetColorTex().get(),
			m_material.get(), Vignette);
		screenData.CopyColorTex();
	}

}

void PP_ColorCollection::Editor_ImGui()
{
	KdPostProcessBase::Editor_ImGui();

	ImGui::Checkbox(u8"グレースケール有効", &m_isGrayscale);
	ImGui::Checkbox(u8"セピア有効", &m_isSepia);
	ImGui::Checkbox(u8"白黒グラデーション有効", &m_isTwoGradiation);
	if (m_isTwoGradiation)
	{
		ImGui::SliderFloat(u8"閾値", &m_twoGradiationValue,0.01f,1.0f);
	}
	ImGui::Checkbox(u8"ビネット有効", &m_isVignette);
	if (m_isVignette)
	{
		ImGui::DragFloat(u8"ビネットの係りの強さ", &m_vignetteIntenPow,0.01f);
		ImGui::DragFloat(u8"ビネットマスクの強さ", &m_vignetteMaskPow,0.01f);
	}

	// セピア変換は白黒になっていることが前提
	if(m_isSepia){
		m_isGrayscale = true;
	}
	if (m_isTwoGradiation)
	{
		m_isGrayscale = true;
	}
}
