#pragma once

#include "KdDirectX.h"

//=================================================
// 
// �u�����h�X�e�[�g
// 
//=================================================
class KdBlendState
{
public:

	// �u�����h�Ȃ����[�h�ō쐬
	void Create_NoBlend();

	// �������u�����h���[�h�ō쐬
	void Create_Alpha();

	// ���Z�u�����h���[�h�ō쐬
	void Create_Add();

	// ��Z�u�����h���[�h�ō쐬
	void Create_Multiply();

	// �f�o�C�X�ɃZ�b�g
	void SetToDevice(const Math::Color& blendFactor = { 0,0,0,0 }, UINT SampleMask = 0xFFFFFFFF);

	void Release()
	{
		m_bs.Reset();
	}

private:

	ComPtr<ID3D11BlendState> m_bs;

	static std::stack<std::shared_ptr<KdBlendState>> s_blendStateStack;
};

//=================================================
// 
// �T���v���[�X�e�[�g
// 
//=================================================
class KdSamplerState
{
public:

	enum class FilterModes
	{
		Point,			// �|�C���g�t�B���^
		Linear,			// ���`�t�B���^
		Anisotropic,	// �ٕ����t�B���^
	};

	enum class AddressingModes
	{
		Wrap,			// ���b�v���[�h
		Clamp,			// �N�����v���[�h
	};


	void Create(FilterModes filterType, UINT maxAnisotropy, AddressingModes addressingMode, bool comparisonModel);

	// �f�o�C�X�ɃZ�b�g
	void SetToDevice(UINT slotNo);

	void Release()
	{
		m_ss.Reset();
	}

private:

	ComPtr<ID3D11SamplerState> m_ss;

};

//=================================================
// 
// �[�x�X�e���V���X�e�[�g
// 
//=================================================
class KdDepthStencilState
{
public:

	void Create(bool zEnable, bool zWriteEnable);

	// �f�o�C�X�ɃZ�b�g
	void SetToDevice(UINT stencilRef = 0);

	void Release()
	{
		m_ds.Reset();
	}

private:

	ComPtr<ID3D11DepthStencilState> m_ds;

};

//=================================================
// 
// ���X�^���C�U�X�e�[�g
// 
//=================================================
class KdRasterizerState
{
public:

	void Create(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, bool depthClipEnable, bool scissorEnable);

	// �f�o�C�X�ɃZ�b�g
	void SetToDevice();

	void Release()
	{
		m_rs.Reset();
	}

private:

	ComPtr<ID3D11RasterizerState> m_rs;

};