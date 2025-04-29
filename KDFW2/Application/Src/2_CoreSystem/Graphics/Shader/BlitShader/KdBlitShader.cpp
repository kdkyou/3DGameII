#include "KdBlitShader.h"

struct Vertex
{
	KdVector3 Pos;
	KdVector2 UV;
};


bool KdBlitShader::Initialize()
{
	//-------------------------------------
	// ���_�V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "BlitShader_VS.csh"

		// ���_�V�F�[�_�[�쐬
		if (FAILED(D3D.GetDev()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_VS))) {
			assert(0 && "���_�V�F�[�_�[�쐬���s");
			Release();
			return false;
		}

		// �P���_�̏ڍׂȏ��
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0,  12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		// ���_���̓��C�A�E�g�쐬
		if (FAILED(D3D.GetDev()->CreateInputLayout(
			&layout[0],			// ���̓G�������g�擪�A�h���X
			(UINT)layout.size(),		// ���̓G�������g��
			&compiledBuffer[0],				// ���_�o�b�t�@�̃o�C�i���f�[�^
			sizeof(compiledBuffer),			// ��L�̃o�b�t�@�T�C�Y
			&m_inputLayout))					// 
		) {
			assert(0 && "CreateInputLayout���s");
			Release();
			return false;
		}

	}

	//-------------------------------------
	// �s�N�Z���V�F�[�_
	//-------------------------------------
	{
		// �R���p�C���ς݂̃V�F�[�_�[�w�b�_�[�t�@�C�����C���N���[�h
		#include "BlitShader_PS.csh"

		if (FAILED(D3D.GetDev()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, &m_PS))) {
			assert(0 && "�s�N�Z���V�F�[�_�[�쐬���s");
			Release();
			return false;
		}
	}

	//-------------------------------------
	// ���_�쐬
	//-------------------------------------

	Vertex vertex[] = {
		{ {-1,-1, 0}, { 0, 1} },	// ����
		{ {-1, 1, 0}, { 0, 0} },	// ����
		{ { 1,-1, 0}, { 1, 1} },	// �E��
		{ { 1, 1, 0}, { 1, 0} },	// �E��
	};

	m_vb = std::make_shared<KdBuffer>();
	m_vb->Create(D3D11_BIND_VERTEX_BUFFER, sizeof(Vertex) * 4, D3D11_USAGE_DYNAMIC, 0, nullptr);
	m_vb->WriteData(vertex, sizeof(vertex));

	return true;
}

void KdBlitShader::Draw(KdTexture* srcTexture, KdRenderTexture* destTexture, KdMaterial* material, uint32_t passIdx)
{
	// �X�e�[�g�̃Z�b�g
	KdShaderManager::GetInstance().m_ds_ZDisable_ZWriteDisable->SetToDevice();

	// �}�e���A��������ꍇ�́A�������̃V�F�[�_�[���g�p����
	if (material != nullptr)
	{
		// ���_�V�F�[�_�������ꍇ�́A�W���̂��̂��g�p����
		if (material->IsValidVS(passIdx) == false)
		{
			// ���_�V�F�[�_���Z�b�g
			D3D.GetDevContext()->VSSetShader(m_VS.Get(), 0, 0);
			// ���_���C�A�E�g���Z�b�g
			D3D.GetDevContext()->IASetInputLayout(m_inputLayout.Get());
		}

		// �s�N�Z���V�F�[�_���Z�b�g
		material->SetToDevice(passIdx);
	}
	else
	{
		// ���_�V�F�[�_���Z�b�g
		D3D.GetDevContext()->VSSetShader(m_VS.Get(), 0, 0);
		// ���_���C�A�E�g���Z�b�g
		D3D.GetDevContext()->IASetInputLayout(m_inputLayout.Get());
		// �s�N�Z���V�F�[�_���Z�b�g
		D3D.GetDevContext()->PSSetShader(m_PS.Get(), 0, 0);

	}

	// �v���~�e�B�u�g�|���W�[���Z�b�g
	D3D.GetDevContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �����_�[�^�[�Q�b�g��ݒ�
	if (destTexture != nullptr)
	{
		D3D.SetRenderTargets({ }, nullptr);
	}

	// �e�N�X�`���Z�b�g
	if (srcTexture != nullptr)
	{
		D3D.SetTextureToPS(0, srcTexture);
	}

	// �����_�[�^�[�Q�b�g��ݒ�
	if (destTexture != nullptr)
	{
		D3D.SetRenderTargets({ destTexture }, nullptr);
	}

	// ���_�o�b�t�@�[���f�o�C�X�փZ�b�g
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		D3D.GetDevContext()->IASetVertexBuffers(0, 1, m_vb->GetBuffer().GetAddressOf(), &stride, &offset);
	}

	// �`��
	D3D.GetDevContext()->Draw(4, 0);
}


void KdBlitShader::Release()
{
    m_VS.Reset();
    m_inputLayout.Reset();
    m_PS.Reset();

	m_vb = nullptr;
}
