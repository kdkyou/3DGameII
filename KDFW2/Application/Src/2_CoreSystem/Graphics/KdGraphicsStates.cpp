#include "KdGraphicsStates.h"

void KdBlendState::Create_NoBlend()
{
	Release();

	D3D11_BLEND_DESC desc = {};
	// �L��
	desc.RenderTarget[0].BlendEnable = TRUE;
	// �������݃}�X�N�@R,G,B,A�S�ďo�͂���ꍇ��ALL
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	// �u�����h�Ȃ�
	{
		desc.RenderTarget[0].BlendEnable = FALSE;

		// �F�̍������@
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		// �A���t�@�̍������@
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	}

	// �X�e�[�g�I�u�W�F�N�g�쐬
	D3D.GetDev()->CreateBlendState(&desc, &m_bs);
}

void KdBlendState::Create_Alpha()
{
	Release();

	D3D11_BLEND_DESC desc = {};
	// �L��
	desc.RenderTarget[0].BlendEnable = TRUE;
	// �������݃}�X�N�@R,G,B,A�S�ďo�͂���ꍇ��ALL
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	// �������u�����h
	{
		// �F�̍������@
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		// �A���t�@�̍������@
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	}

	// �X�e�[�g�I�u�W�F�N�g�쐬
	D3D.GetDev()->CreateBlendState(&desc, &m_bs);
}

void KdBlendState::Create_Add()
{
	Release();

	D3D11_BLEND_DESC desc = {};
	// �L��
	desc.RenderTarget[0].BlendEnable = TRUE;
	// �������݃}�X�N�@R,G,B,A�S�ďo�͂���ꍇ��ALL
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	// ���Z�u�����h
	{
		// �F�̍������@
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		// �A���t�@�̍������@
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	}

	// �X�e�[�g�I�u�W�F�N�g�쐬
	if (FAILED(D3D.GetDev()->CreateBlendState(&desc, &m_bs)))return;
}

void KdBlendState::Create_Multiply()
{
	Release();

	D3D11_BLEND_DESC desc = {};
	// �L��
	desc.RenderTarget[0].BlendEnable = TRUE;
	// �������݃}�X�N�@R,G,B,A�S�ďo�͂���ꍇ��ALL
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	desc.IndependentBlendEnable = FALSE;
	desc.AlphaToCoverageEnable = FALSE;

	// ���Z�u�����h
	{
		// �F�̍������@
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
		// �A���t�@�̍������@
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	}

	// �X�e�[�g�I�u�W�F�N�g�쐬
	if (FAILED(D3D.GetDev()->CreateBlendState(&desc, &m_bs)))return;
}

void KdBlendState::SetToDevice(const Math::Color& blendFactor, UINT SampleMask)
{
	D3D.GetDevContext()->OMSetBlendState(m_bs.Get(), blendFactor, SampleMask);
}



void KdSamplerState::Create(FilterModes filterType, UINT maxAnisotropy, AddressingModes addressingMode, bool comparisonModel)
{
	Release();

	D3D11_SAMPLER_DESC desc = {};
	// �t�B���^
	switch (filterType) {
		// Point
	case FilterModes::Point:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
		// Linear
	case FilterModes::Linear:
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
		// Anisotropic
	case FilterModes::Anisotropic:
		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		desc.MaxAnisotropy = maxAnisotropy;
		break;
	}

	// �A�h���b�V���O���[�h
	switch (addressingMode) {
		// Wrap
	case AddressingModes::Wrap:
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
		// Clamp
	case AddressingModes::Clamp:
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	}

	// ��r���[�h�T���v��
	if (comparisonModel) {
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;	// �Ƃ肠��������͐��`�t�B���^�Œ��
	}
	// �ʏ�T���v��
	else {
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	}

	// 
	desc.MipLODBias = 0;
	desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	// �X�e�[�g�I�u�W�F�N�g�쐬
	ComPtr<ID3D11SamplerState> state = nullptr;
	D3D.GetDev()->CreateSamplerState(&desc, &m_ss);
}

void KdSamplerState::SetToDevice(UINT slotNo)
{
	D3D.GetDevContext()->VSSetSamplers(slotNo, 1, m_ss.GetAddressOf());
	D3D.GetDevContext()->PSSetSamplers(slotNo, 1, m_ss.GetAddressOf());
	D3D.GetDevContext()->GSSetSamplers(slotNo, 1, m_ss.GetAddressOf());
	D3D.GetDevContext()->CSSetSamplers(slotNo, 1, m_ss.GetAddressOf());
}




void KdDepthStencilState::Create(bool zEnable, bool zWriteEnable)
{
	Release();

	// Z�o�b�t�@�̓���ݒ�
	D3D11_DEPTH_STENCIL_DESC desc = {};

	desc.DepthEnable = zEnable;							// �[�x�e�X�g���g�p����
	desc.DepthWriteMask = zWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	desc.StencilEnable = FALSE;
	desc.StencilReadMask = 0;
	desc.StencilWriteMask = 0;

	// �X�e�[�g�I�u�W�F�N�g���쐬
	D3D.GetDev()->CreateDepthStencilState(&desc, &m_ds);
}


void KdDepthStencilState::SetToDevice(UINT stencilRef)
{
	D3D.GetDevContext()->OMSetDepthStencilState(m_ds.Get(), stencilRef);
}




void KdRasterizerState::Create(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool depthClipEnable, bool scissorEnable)
{
	Release();

	// ���X�^���C�Y�̓���ݒ�
	D3D11_RASTERIZER_DESC desc = {};
	desc.FillMode = fillMode;
	desc.CullMode = cullMode;
	desc.FrontCounterClockwise = FALSE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0;
	desc.SlopeScaledDepthBias = 0;
	desc.DepthClipEnable = depthClipEnable;
	desc.ScissorEnable = scissorEnable;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	// �X�e�[�g�I�u�W�F�N�g�쐬
	ComPtr < ID3D11RasterizerState> state = nullptr;
	D3D.GetDev()->CreateRasterizerState(&desc, &m_rs);;
}

// �f�o�C�X�ɃZ�b�g

void KdRasterizerState::SetToDevice()
{
	D3D.GetDevContext()->RSSetState(m_rs.Get());
}

