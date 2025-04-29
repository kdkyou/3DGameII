#include "KdSkinningShader.h"

bool KdSkinningShader::Initialize()
{
	//-------------------------------------
	// ���_�V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "Skinning_VS.csh"

		// ���_�V�F�[�_�[�쐬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "���_�V�F�[�_�[�쐬���s");
			Release();
			return false;
		}

		// �P���_�̏ڍׂȏ��
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout =
		{
			{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT,		1,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		2,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SKININDEX",  0, DXGI_FORMAT_R16G16B16A16_UINT,	3,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SKINWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	4,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ���_���̓��C�A�E�g�쐬
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&inputLayout[0],			// ���̓G�������g�擪�A�h���X
			(UINT)inputLayout.size(),		// ���̓G�������g��
			&compiledBuffer[0],				// ���_�o�b�t�@�̃o�C�i���f�[�^
			sizeof(compiledBuffer),			// ��L�̃o�b�t�@�T�C�Y
			&m_inputLayout))					// 
		) {
			assert(0 && "CreateInputLayout���s");
			Release();
			return false;
		}

		// �X�g���[���A�E�g�v�b�g���W�I���g���V�F�[�_�[
		std::vector<D3D11_SO_DECLARATION_ENTRY> soEntrys =
		{
		  { 0, "SV_Position",	0, 0, 3, kStreamOutputSlotTypes::Pos },
		  { 0, "TANGENT",		0, 0, 3, kStreamOutputSlotTypes::Tangent },
		  { 0, "NORMAL",		0, 0, 3, kStreamOutputSlotTypes::Normal },
		};

		std::vector<uint32_t> strides =
		{
			12,
			12,
			12,
		};

		if (FAILED(D3D.GetDev()->CreateGeometryShaderWithStreamOutput(compiledBuffer, sizeof(compiledBuffer),
			&soEntrys[0], (uint32_t)soEntrys.size(),
			&strides[0], (uint32_t)strides.size(),
			D3D11_SO_NO_RASTERIZED_STREAM,
			nullptr,
			&m_GS)))
		{
			assert(0 && "CreateGeometryShaderWithStreamOutput ���s");
			Release();
			return false;
		}
	}

	return true;
}

void KdSkinningShader::Release()
{
	m_VS.Reset();
	m_GS.Reset();
	m_inputLayout.Reset();
}


void KdSkinningShader::DrawToStreamOutput(uint32_t vertexCount, const std::vector<ID3D11Buffer*>& soVBs)
{
	// �v���~�e�B�u�g�|���W�[���Z�b�g(POINTLIST�c���_�P��)
	D3D	.GetDevContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	// ���_�V�F�[�_���Z�b�g
	D3D.GetDevContext()->VSSetShader(m_VS.Get(), 0, 0);
	// �W�I���g���V�F�[�_���Z�b�g
	D3D.GetDevContext()->GSSetShader(m_GS.Get(), 0, 0);
	// �s�N�Z���V�F�[�_�[�͎g�p���Ȃ�
	D3D.GetDevContext()->PSSetShader(nullptr, 0, 0);

	// ���_���C�A�E�g���Z�b�g
	D3D.GetDevContext()->IASetInputLayout(m_inputLayout.Get());

	// Stream Output�o�̓o�b�t�@���Z�b�g
	uint32_t soOffset[D3D11_SO_BUFFER_SLOT_COUNT] = {0, 0, 0, 0};
	D3D.GetDevContext()->SOSetTargets((uint32_t)soVBs.size(), &soVBs[0], &soOffset[0]);

	// �`��
	D3D.GetDevContext()->Draw(vertexCount, 0);

	// �W�I���g���V�F�[�_�[������
	D3D.GetDevContext()->GSSetShader(nullptr, 0, 0);

	// SO�o�̓o�b�t�@������
	static ID3D11Buffer* nullBuffer[D3D11_SO_BUFFER_SLOT_COUNT] = { nullptr, nullptr, nullptr, nullptr };
	D3D.GetDevContext()->SOSetTargets((uint32_t)soVBs.size(), nullBuffer, &soOffset[0]);
}

