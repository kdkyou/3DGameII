#include "KdScreenData.h"

#include "KdFramework.h"

void KdScreenData::Rendering(const std::vector<KdGameObject*>& gameObjects, const std::shared_ptr<KdRenderTexture>& outputTex)
{
	KdRenderSettings& renderSettings = KdRenderSettings::GetInstance();

	// ��ƃf�[�^�̃N���A
	ResetTempData();

	//+++++++++++++++++++++++++++
	// 
	// �`�� �O ����
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
	// ���C�g���X�V
	//+++++++++++++++++++++++++++

	// ���C�g�������Z�b�g����
	KdShaderManager::GetInstance().m_cbLight->EditCB().Reset();

	// ����
	KdShaderManager::GetInstance().m_cbLight->EditCB().AmbientLight = renderSettings.m_ambientColor;

	// �����t�H�O
	KdShaderManager::GetInstance().m_cbLight->EditCB().DistanceFogColor = renderSettings.m_distanceFogColor;
	KdShaderManager::GetInstance().m_cbLight->EditCB().DistanceFogDensity = renderSettings.m_distanceFogDensity;

	// �e���L���ȕ��s���̋L���p
	KdLightComponent* shadowCasterDirLight = nullptr;

	for (KdLightComponent* light : KdFramework::GetInstance().m_renderingData.m_lights)
	{
		// ���s���̏ꍇ
		if (shadowCasterDirLight == nullptr &&
			light->GetCastShadow() &&
			light->GetLightType() == KdLightComponent::LightTypes::Directional)
		{
			shadowCasterDirLight = light;
		}

		// ���C�g�̏����A�萔�o�b�t�@�ɏ�������
		light->WriteLightDataToCBuffer();
	}

	// IBL�e�N�X�`���Z�b�g
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::IBL, renderSettings.m_IBLTex.get());

	//+++++++++++++++++++++++++++
	// 
	// ���s�� �V���h�E�}�b�v�쐬�`��
	// 
	//+++++++++++++++++++++++++++
	if (shadowCasterDirLight != nullptr)
	{
		KdShaderManager::GetInstance().m_ds_ZEnable_ZWriteEnable->SetToDevice();
		KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();
		KdShaderManager::GetInstance().m_rs_CullBack->SetToDevice();

		// ���C�g�̍s��
		KdMatrix mLight = shadowCasterDirLight->GetGameObject()->GetTransform()->GetWorldMatrix();
		// ���ˉe�s��̍쐬
		KdMatrix mProj = DirectX::XMMatrixOrthographicLH(100, 100, 0.1f, 100);

		// ���C�g���J�����Ƃ��ĕ`�悷�邽�߁A�J�����Ƀ��C�g�̍s�����������
		KdShaderManager::GetInstance().m_cbPerCamera->EditCB().SetDatas(mLight, mProj);
		KdShaderManager::GetInstance().m_cbPerCamera->WriteWorkData();


		// �`���e�N�X�`���Z�b�g
		D3D.SetRenderTargets(
			{ m_dirLightShadowMap.get() },
			m_dirLightShadowMap->GetDepth().get());		// Z�o�b�t�@

		m_dirLightShadowMap->Clear({ 1,1,1 }, 1.0f, 0);

		// �S�I�u�W�F�N�g�`��
		for (auto&& comp : m_drawList)
		{
			comp->Draw(true, KdShader::PassTags::ShadowCaster);
		}

		// �V���h�E�}�b�v�̏����Z�b�g
		KdShaderManager::GetInstance().m_cbLight->EditCB().DL_ShadowPower = 1.0f;
		KdShaderManager::GetInstance().m_cbLight->EditCB().DL_mLightVP = mLight.Invert() * mProj;

		// Z�o�b�t�@���A���̓e�N�X�`���Ƃ��ăZ�b�g
		D3D.SetRenderTargets({}, nullptr);
		D3D.SetTextureToPS(KdShaderManager::TextureSlots::DirLightShadowMap, m_dirLightShadowMap->GetDepth().get());
	}


	// �J�����̒萔�o�b�t�@��������
	m_camera->WriteCameraCBuffer();
	// ���C�g�̒萔�o�b�t�@��������
	KdShaderManager::GetInstance().m_cbLight->WriteWorkData();

	//+++++++++++++++++++++++++++
	// 
	// �s������ �`�揈��
	// 
	//+++++++++++++++++++++++++++

	KdShaderManager::GetInstance().m_ds_ZEnable_ZWriteEnable->SetToDevice();
	KdShaderManager::GetInstance().m_bs_Copy->SetToDevice();
	KdShaderManager::GetInstance().m_rs_CullBack->SetToDevice();

	// �`���e�N�X�`���Z�b�g
	D3D.SetRenderTargets(
		{
			m_color.get(),				// RT0�F�J���[�o�b�t�@
			m_normal.get()				// RT1�F�@���}�b�v
		},
		m_color->GetDepth().get());		// Z�o�b�t�@

	// RT��Z�o�b�t�@�N���A���N���A
	m_color->Clear({ 0.3f, 0.3f, 1.0f, 1.0f });
	m_normal->Clear({ 0.0f, 0.0f, 0.0f, 1.0f });

	// �`��
	for (auto&& comp : m_drawList)
	{
		comp->Draw(true, KdShader::PassTags::Forward);
	}

	// �e�o�b�t�@�̓��e���R�s�[����
	CopyColorTex();
	CopyNormalTex();
//	m_color->CopyTo(*m_inputColor);
//	m_normal->CopyTo(*m_inputNormal);

	// �e�o�b�t�@�̃R�s�[���A���̓e�N�X�`���Ƃ��ăZ�b�g
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
	// �������� �`�揈��
	// 
	//+++++++++++++++++++++++++++

	KdShaderManager::GetInstance().m_ds_ZEnable_ZWriteDisable->SetToDevice();
	KdShaderManager::GetInstance().m_bs_Alpha->SetToDevice();

	/*
	// ��������ʂ�`���Ƃ��ăZ�b�g
	D3D.SetRenderTargets(
		{
			m_transparent.get(),	// RT0�F�J���[�o�b�t�@
			m_normal.get()			// RT1�F�@���}�b�v
		},
		m_transparent->GetDepth().get());		// Z�o�b�t�@
	*/

	// �`��
	for (auto&& comp : m_drawList)
	{
		comp->Draw(false, KdShader::PassTags::Forward);
	}

	// �e�o�b�t�@�̃R�s�[���쐬
	m_color->CopyTo(*m_inputColor);
	m_normal->CopyTo(*m_inputNormal);

	// �e�o�b�t�@�̃R�s�[���A���̓e�N�X�`���Ƃ��ăZ�b�g
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
	// �o��
	// 
	//+++++++++++++++++++++++++++
	if (outputTex != nullptr)
	{
		// �������o�b�t�@���o�b�N�o�b�t�@�փR�s�[(�`��)
		KdShaderManager::GetInstance().m_blitShader.Draw(m_color.get(), outputTex.get());
	}


	// ���̓e�N�X�`���̉���
	D3D.SetTextureToVS(0, nullptr);
	D3D.SetTextureToPS(0, nullptr);
	D3D.SetTextureToGS(0, nullptr);

	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Color, nullptr);
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::Depth, nullptr);
	D3D.SetTextureToPS(KdShaderManager::TextureSlots::DirLightShadowMap, nullptr);
}

void KdPostProcessBase::Editor_ImGui()
{
	ImGui::Checkbox(u8"�L��", &m_enable);
}
