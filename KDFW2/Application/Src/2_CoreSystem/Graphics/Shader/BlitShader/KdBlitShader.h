#pragma once

#include "../../KdDirectX.h"

class KdBlitShader
{
public:

	// ������
	bool Initialize();

	// ���
	void Release();

	// ���݂�RT�S�̂ɁA��`��`�悷��
	// �Etexture �c Slot0�ɃZ�b�g����e�N�X�`��
	// �Ematerial �c �w�肷��ƁA�q�̃}�e���A���̃V�F�[�_�[���g�p����Bnull���ƕW���V�F�[�_�[���g�p����
	// �EpassIdx �c material�̃p�X�ԍ����w��
	void Draw(KdTexture* srcTexture, KdRenderTexture* destTexture, KdMaterial* material = nullptr, uint32_t passIdx = 0);

private:


	// 3D���f���p�V�F�[�_
	ComPtr<ID3D11VertexShader>	m_VS = nullptr;				// ���_�V�F�[�_�[
	ComPtr<ID3D11InputLayout>	m_inputLayout = nullptr;		// ���_���̓��C�A�E�g
	ComPtr<ID3D11PixelShader>	m_PS = nullptr;				// �s�N�Z���V�F�[�_�[

	// ���_�o�b�t�@
	std::shared_ptr<KdBuffer>	m_vb;
};