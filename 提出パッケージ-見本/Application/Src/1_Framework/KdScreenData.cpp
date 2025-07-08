#include "KdScreenData.h"

#include "KdFramework.h"

void KdScreenData::Rendering(const std::vector<KdGameObject*>& gameObjects, const std::shared_ptr<KdRenderTexture>& outputTex)
{
	KdRenderSettings& renderSettings = KdRenderSettings::GetInstance();

	// 作業データのクリア
	ResetTempData();

	//+++++++++++++++++++++++++++
	// 
	// 描画 前 処理
	// 
	//+++++++++++++++++++++++++++
	for (auto&& gameObj : gameObjects)
	{
		for (auto&& comp : gameObj->GetComponentList())
		{
			comp->PreDraw();
		}
	}


	//+++++++++++++++++++++++++++
	// ライト情報更新
	//+++++++++++++++++++++++++++

	// ライト情報をリセットする
	KdShaderManager::GetInstance().m_cbLight->EditCB().Reset();

	// 環境光
	KdShaderManager::GetInstance().m_cbLight->EditCB().AmbientLight = renderSettings.m_ambientColor;

	// 距離フォグ
	KdShaderManager::GetInstance().m_cbLight->EditCB().DistanceFogColor = renderSettings.m_distanceFogColor;
	KdShaderManager::GetInstance().m_cbLight->EditCB().DistanceFogDensity = renderSettings.m_distanceFogDensity;

	// 影が有効な平行光の記憶用
	KdLightComponent* shadowCasterDirLight = nullptr;

	for (KdLightComponent* light : KdFramework::GetInstance().m_renderingData.m_lights)
	{
		// 平行光の場合
		if (shadowCasterDirLight == nullptr &&
			light->GetCastShadow() &&
			light->GetLightType() == KdLightComponent::LightTypes::Directional)
		{
			shadowCasterDirLight = light;
		}

		// ライトの情報を、定数バッファに書き込む
		light->WriteLightDataToCBuffer();
	}

	// IBLテクスチャセット
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::IBL, renderSettings.m_IBLTex.get());

	//+++++++++++++++++++++++++++
	// 
	// 平行光 シャドウマップ作成描画
	// 
	//+++++++++++++++++++++++++++
	if (shadowCasterDirLight != nullptr)
	{
		KdShaderManager::GetInstance().m_ds_ZEnable_ZWriteEnable->SetToDevice();
		KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();
		KdShaderManager::GetInstance().m_rs_CullBack->SetToDevice();

		// ライトの行列
		KdMatrix mLight = shadowCasterDirLight->GetGameObject()->GetTransform()->GetWorldMatrix();
		// 正射影行列の作成
		KdMatrix mProj = DirectX::XMMatrixOrthographicLH(100, 100, 0.1f, 100);

		// ライトをカメラとして描画するため、カメラにライトの行列を書き込む
		KdShaderManager::GetInstance().m_cbPerCamera->EditCB().SetDatas(mLight, mProj);
		KdShaderManager::GetInstance().m_cbPerCamera->WriteWorkData();


		// 描画先テクスチャセット
		D3D.SetRenderTargets(
			{ m_dirLightShadowMap.get() },
			m_dirLightShadowMap->GetDepth().get());		// Zバッファ

		m_dirLightShadowMap->Clear({ 1,1,1 }, 1.0f, 0);

		// 全オブジェクト描画
		for (auto&& comp : m_drawList)
		{
			comp->Draw(true, KdShader::PassTags::ShadowCaster);
		}

		// シャドウマップの情報をセット
		KdShaderManager::GetInstance().m_cbLight->EditCB().DL_ShadowPower = 1.0f;
		KdShaderManager::GetInstance().m_cbLight->EditCB().DL_mLightVP = mLight.Invert() * mProj;

		// Zバッファを、入力テクスチャとしてセット
		D3D.SetRenderTargets({}, nullptr);
		D3D.SetTextureToPS(KdShaderManager::TextureSlots::DirLightShadowMap, m_dirLightShadowMap->GetDepth().get());
	}


	// カメラの定数バッファ書き込み
	m_camera->WriteCameraCBuffer();
	// ライトの定数バッファ書き込み
	KdShaderManager::GetInstance().m_cbLight->WriteWorkData();

	//+++++++++++++++++++++++++++
	// 
	// 不透明物 描画処理
	// 
	//+++++++++++++++++++++++++++

	KdShaderManager::GetInstance().m_ds_ZEnable_ZWriteEnable->SetToDevice();
	KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();
	KdShaderManager::GetInstance().m_rs_CullBack->SetToDevice();

	// 描画先テクスチャセット
	D3D.SetRenderTargets(
		{
			m_color.get(),				// RT0：カラーバッファ
			m_normal.get()				// RT1：法線マップ
		},
		m_color->GetDepth().get());		// Zバッファ

	// RTとZバッファクリアをクリア
	m_color->Clear({ 0.3f, 0.3f, 1.0f, 1.0f });
	m_normal->Clear({ 0.0f, 0.0f, 0.0f, 1.0f });

	// 描画
	for (auto&& comp : m_drawList)
	{
		comp->Draw(true, KdShader::PassTags::Forward);
	}

	// 各バッファの内容をコピーする
	CopyColorTex();
	CopyNormalTex();
//	m_color->CopyTo(*m_inputColor);
//	m_normal->CopyTo(*m_inputNormal);

	// 各バッファのコピーを、入力テクスチャとしてセット
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Color, m_inputColor.get());
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Normal, m_inputNormal.get());
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Depth, m_inputColor->GetDepth().get());

	//<><><><><><><><><><><><><>
	// Post Process
	//<><><><><><><><><><><><><>

	KdShaderManager::GetInstance().m_ds_ZDisable_ZWriteDisable->SetToDevice();
	KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();

	for (auto&& pp : KdRenderSettings::GetInstance().m_postProcesses)
	{
		if (pp->GetTiming() == KdPostProcessBase::Timings::AfterOpaque)
		{
			pp->Execute(*this);
		}
	}

	//+++++++++++++++++++++++++++
	// 
	// 半透明物 描画処理
	// 
	//+++++++++++++++++++++++++++

	KdShaderManager::GetInstance().m_ds_ZEnable_ZWriteDisable->SetToDevice();
	KdShaderManager::GetInstance().m_bs_Alpha->SetToDevice();

	/*
	// 半透明画面を描画先としてセット
	D3D.SetRenderTargets(
		{
			m_transparent.get(),	// RT0：カラーバッファ
			m_normal.get()			// RT1：法線マップ
		},
		m_transparent->GetDepth().get());		// Zバッファ
	*/

	// 描画
	for (auto&& comp : m_drawList)
	{
		comp->Draw(false, KdShader::PassTags::Forward);
	}

	// 各バッファのコピーを作成
	m_color->CopyTo(*m_inputColor);
	m_normal->CopyTo(*m_inputNormal);

	// 各バッファのコピーを、入力テクスチャとしてセット
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Color, m_inputColor.get());
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Normal, m_inputNormal.get());
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Depth, m_inputColor->GetDepth().get());

	//<><><><><><><><><><><><><>
	// Post Process
	//<><><><><><><><><><><><><>

	KdShaderManager::GetInstance().m_ds_ZDisable_ZWriteDisable->SetToDevice();
	KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();

	for (auto&& pp : KdRenderSettings::GetInstance().m_postProcesses)
	{
		if (pp->GetTiming() == KdPostProcessBase::Timings::AfterTransparent)
		{
			pp->Execute(*this);
		}
	}


	//+++++++++++++++++++++++++++
	// 
	// 出力
	// 
	//+++++++++++++++++++++++++++
	if (outputTex != nullptr)
	{
		// 半透明バッファをバックバッファへコピー(描画)
		KdShaderManager::GetInstance().m_blitShader.Draw(m_color.get(), outputTex.get());
	}


	// 入力テクスチャの解除
	D3D.SetTextureToVS(0, nullptr);
	D3D.SetTextureToPS(0, nullptr);
	D3D.SetTextureToGS(0, nullptr);

	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Color, nullptr);
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Depth, nullptr);
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::DirLightShadowMap, nullptr);
}

void KdPostProcessBase::Editor_ImGui()
{
	ImGui::Checkbox(u8"有効", &m_enable);
}
