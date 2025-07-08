#include "KdShaderManager.h"

void KdShaderManager::Init()
{

	//============================================
	// シェーダ
	//============================================
	m_blitShader.Initialize();
	m_skinningShader.Initialize();

	//============================================
	// 定数バッファ
	//============================================

	// 描画単位データ
	{
		m_cbPerDraw = std::make_shared<KdConstantBufferType<CBPerDraw>>();
		m_cbPerDraw->CreateConstantBuffer();

		m_cbPerDraw->SetVS(ConstantBufferSlots::PerDraw);
		m_cbPerDraw->SetPS(ConstantBufferSlots::PerDraw);
		m_cbPerDraw->SetGS(ConstantBufferSlots::PerDraw);
	}

	// カメラ単位データ
	{
		m_cbPerCamera = std::make_shared<KdConstantBufferType<CBPerCamera>>();
		m_cbPerCamera->CreateConstantBuffer();

		m_cbPerCamera->SetVS(ConstantBufferSlots::PerCamera);
		m_cbPerCamera->SetPS(ConstantBufferSlots::PerCamera);
		m_cbPerCamera->SetGS(ConstantBufferSlots::PerCamera);

		m_cbPerCamera->EditCB().SetCamera(KdMatrix::CreateTranslation(0, 0, -5));

		m_cbPerCamera->EditCB().SetPerspectiveFov(80 * KdDeg2Rad, 1920/1080.0f, 0.01f, 1000);

		m_cbPerCamera->WriteWorkData();
	}

	// ライトデータ
	{
		m_cbLight = std::make_shared<KdConstantBufferType<CBLight>>();
		m_cbLight->CreateConstantBuffer();

		m_cbLight->SetVS(ConstantBufferSlots::Light);
		m_cbLight->SetPS(ConstantBufferSlots::Light);
		m_cbLight->SetGS(ConstantBufferSlots::Light);

		m_cbLight->WriteWorkData();
	}

	// システムデータ
	{
		m_cbSystem = std::make_shared<KdConstantBufferType<CBSystem>>();
		m_cbSystem->CreateConstantBuffer();

		m_cbSystem->SetVS(ConstantBufferSlots::System);
		m_cbSystem->SetPS(ConstantBufferSlots::System);
		m_cbSystem->SetGS(ConstantBufferSlots::System);

		m_cbSystem->WriteWorkData();
		
	}

	/*
	// カメラ
	m_cb7_Camera.Create();
	D3D.GetDevContext()->VSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 7,   1, m_cb7_Camera.GetAddress());

	// 初期ビュー行列
	m_cb7_Camera.Work().mV.CreateTranslation(0, 0, -5);
	m_cb7_Camera.Work().mV.Inverse();

	// 初期射影行列
	m_cb7_Camera.Work().mP = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60), D3D.GetBackBuffer()->GetAspectRatio(), 0.01f, 1000);

	m_cb7_Camera.Write();
	// ライト
	m_cb8_Light.Create();
	D3D.GetDevContext()->VSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());
	D3D.GetDevContext()->PSSetConstantBuffers( 8,	1, m_cb8_Light.GetAddress());
	*/

	//============================================
	// ステート作成
	//============================================

	// サンプラステート作成
	m_ss_Anisotropic_Wrap = std::make_shared<KdSamplerState>();
	m_ss_Anisotropic_Wrap->Create(KdSamplerState::FilterModes::Anisotropic, 4, KdSamplerState::AddressingModes::Wrap, false);
	m_ss_Anisotropic_Clamp = std::make_shared<KdSamplerState>();
	m_ss_Anisotropic_Clamp->Create(KdSamplerState::FilterModes::Anisotropic, 4, KdSamplerState::AddressingModes::Clamp, false);

	m_ss_Linear_Wrap = std::make_shared<KdSamplerState>();
	m_ss_Linear_Wrap->Create(KdSamplerState::FilterModes::Linear, 0, KdSamplerState::AddressingModes::Wrap, false);
	m_ss_Linear_Clamp = std::make_shared<KdSamplerState>();
	m_ss_Linear_Clamp->Create(KdSamplerState::FilterModes::Linear, 0, KdSamplerState::AddressingModes::Clamp, false);

	m_ss_Point_Wrap = std::make_shared<KdSamplerState>();
	m_ss_Point_Wrap->Create(KdSamplerState::FilterModes::Point, 0, KdSamplerState::AddressingModes::Wrap, false);
	m_ss_Point_Clamp = std::make_shared<KdSamplerState>();
	m_ss_Point_Clamp->Create(KdSamplerState::FilterModes::Point, 0, KdSamplerState::AddressingModes::Clamp, false);

	m_ss_Linear_Comparison_Clamp = std::make_shared<KdSamplerState>();
	m_ss_Linear_Comparison_Clamp->Create(KdSamplerState::FilterModes::Linear, 0, KdSamplerState::AddressingModes::Clamp, true);

	// セット
	SetCommonSamplers();

	// デプスステンシルステート作成
	m_ds_ZEnable_ZWriteEnable = std::make_shared<KdDepthStencilState>();
	m_ds_ZEnable_ZWriteEnable->Create(true, true);

	m_ds_ZEnable_ZWriteDisable = std::make_shared<KdDepthStencilState>();
	m_ds_ZEnable_ZWriteDisable->Create(true, false);

	m_ds_ZDisable_ZWriteDisable = std::make_shared<KdDepthStencilState>();
	m_ds_ZDisable_ZWriteDisable->Create(false, false);

	// ラスタライザステート作成
	m_rs_CullBack = std::make_shared<KdRasterizerState>();
	m_rs_CullBack->Create(D3D11_CULL_BACK, D3D11_FILL_SOLID, true, false);

	m_rs_CullNone = std::make_shared<KdRasterizerState>();
	m_rs_CullNone->Create(D3D11_CULL_NONE, D3D11_FILL_SOLID, true, false);

	m_rs_Wireframe = std::make_shared<KdRasterizerState>();
	m_rs_Wireframe->Create(D3D11_CULL_NONE, D3D11_FILL_WIREFRAME, true, false);

	// ブレンドステート作成
	m_bs_Copy = std::make_shared<KdBlendState>();
	m_bs_Copy->Create_NoBlend();

	m_bs_Alpha = std::make_shared<KdBlendState>();
	m_bs_Alpha->Create_Alpha();

	m_bs_Add = std::make_shared<KdBlendState>();
	m_bs_Add->Create_Add();

	m_bs_Multiply = std::make_shared<KdBlendState>();
	m_bs_Multiply->Create_Multiply();
}

void KdShaderManager::Release()
{
	m_blitShader.Release();
	m_skinningShader.Release();

	m_cbPerDraw = nullptr;
	m_cbPerCamera = nullptr;
	m_cbLight = nullptr;
	m_cbSystem = nullptr;

	m_ss_Anisotropic_Wrap = nullptr;
	m_ss_Anisotropic_Clamp = nullptr;

	m_ds_ZEnable_ZWriteEnable = nullptr;
	m_ds_ZEnable_ZWriteDisable = nullptr;
	m_ds_ZDisable_ZWriteDisable = nullptr;

	m_rs_CullBack = nullptr;
	m_rs_CullNone = nullptr;
	m_rs_Wireframe = nullptr;

	m_bs_Copy = nullptr;
	m_bs_Alpha = nullptr;
	m_bs_Add = nullptr;
	m_bs_Multiply = nullptr;
}

void KdShaderManager::SetCommonSamplers()
{
	m_ss_Anisotropic_Wrap->SetToDevice(0);
	m_ss_Anisotropic_Clamp->SetToDevice(1);
	m_ss_Linear_Wrap->SetToDevice(2);
	m_ss_Linear_Clamp->SetToDevice(3);
	m_ss_Point_Wrap->SetToDevice(4);
	m_ss_Point_Clamp->SetToDevice(5);
	m_ss_Linear_Comparison_Clamp->SetToDevice(15);

}
