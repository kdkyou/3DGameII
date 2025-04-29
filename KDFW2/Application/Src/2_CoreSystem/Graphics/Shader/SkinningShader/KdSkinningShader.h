#pragma once

#include "../../KdDirectX.h"

class KdSkinningShader
{
public:

	// ������
	bool Initialize();

	// ���
	void Release();

	// Stream Output�@�\�ŁA���_��`�悷��B
	// �EvertexCount	�F�`�悷�钸�_��
	// �EsoVBs			�FStream Output�o�͐�́A���_�o�b�t�@����
	void DrawToStreamOutput(uint32_t vertexCount, const std::vector<ID3D11Buffer*>& soVBs);

	// �{�[���z�� �萔�o�b�t�@
	struct CBufferData
	{
		KdMatrix		mBoneMatrix[1024];
	};

	// 
	enum kStreamOutputSlotTypes
	{
		Pos = 0,
		Tangent = 1,
		Normal = 2,
	};

private:


	// 3D���f���p�V�F�[�_
	ComPtr<ID3D11VertexShader>		m_VS = nullptr;				// ���_�V�F�[�_�[
	ComPtr<ID3D11GeometryShader>	m_GS = nullptr;				// �W�I���g���V�F�[�_�[
	ComPtr<ID3D11InputLayout>		m_inputLayout = nullptr;		// ���_���̓��C�A�E�g


};