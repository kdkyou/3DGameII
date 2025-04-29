#pragma once

#include "Component/KdComponent.h"
#include "Component/KdModelRendererComponent.h"
#include "Component/KdCameraComponent.h"

class KdPostProcessBase;

//=========================================================
// 
// �P��ʂԂ�̕`��f�[�^
// 
//=========================================================
class KdScreenData
{
public:
	//-------------------------
	// ���
	//-------------------------
	// ��ʂ̕�
	uint32_t GetScreenWidth() const { return m_color->GetWidth(); }
	// ��ʂ̍���
	uint32_t GetScreenHeight() const { return m_color->GetHeight(); }
	// ��ʂ̃A�X�y�N�g��
	float GetScreenAspect() const { return (float)GetScreenWidth() / (float)GetScreenHeight(); }

	// �J���[�e�N�X�`��
	const std::shared_ptr<KdRenderTexture>& GetColorTex() { return m_color; }
	const std::shared_ptr<KdRenderTexture>& GetInputColorTex() { return m_inputColor; }
	// Z�o�b�t�@
	const std::shared_ptr<KdTexture>& GetDepthTex() { return m_color->GetDepth(); }
	// ��ʂ̖@���}�b�v
	const std::shared_ptr<KdRenderTexture>& GetNormalTex() { return m_normal; }
	// ���s�����p�V���h�E�}�b�v
	const std::shared_ptr<KdRenderTexture>& GetDirLightShadowMapTex() { return m_dirLightShadowMap; }

	//-------------------------
	// �`�掞 �ꎞ�g�p�f�[�^
	//-------------------------

	KdCameraComponent* m_camera;

	// �s����3D�`�惊�X�g
	std::vector<KdRendererComponent*>		m_drawList;


	// �`�掞�ꎞ�g�p�f�[�^�̃��Z�b�g
	void ResetTempData()
	{
		m_drawList.clear();
	}

	// �����ݒ�
	void Initialize()
	{
		// �e�����_�[�e�N�X�`���̍쐬
		m_color = std::make_shared<KdRenderTexture>();
		m_color->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, nullptr, DXGI_FORMAT_R24G8_TYPELESS);

		m_normal = std::make_shared<KdRenderTexture>();
		m_normal->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, nullptr, DXGI_FORMAT_UNKNOWN);

		m_inputColor = std::make_shared<KdRenderTexture>();
		m_inputColor->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, 1, nullptr, DXGI_FORMAT_R24G8_TYPELESS);

		m_inputNormal = std::make_shared<KdRenderTexture>();
		m_inputNormal->CreateRenderTexture(D3D.GetWidth(), D3D.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 1, nullptr, DXGI_FORMAT_UNKNOWN);

		m_dirLightShadowMap = std::make_shared<KdRenderTexture>();
		m_dirLightShadowMap->CreateRenderTexture(1024, 1024, DXGI_FORMAT_R8G8B8A8_UNORM, 1, nullptr, DXGI_FORMAT_R32_TYPELESS);

		ResetTempData();
	}

	void Release()
	{
		m_color = nullptr;
		m_normal = nullptr;
		m_inputColor = nullptr;
		m_inputNormal = nullptr;
	}

	//******************
	// �`������s
	//******************
	void Rendering(const std::vector<KdGameObject*>& gameObjects, const std::shared_ptr<KdRenderTexture>& outputTex);

	// ColorTex��InputColorTex�ɃR�s�[����
	void CopyColorTex()
	{
		m_color->CopyTo(*m_inputColor);
	}
	void CopyNormalTex()
	{
		m_normal->CopyTo(*m_inputNormal);
	}

private:

	// �J���[�o�b�t�@ & Z�o�b�t�@
	std::shared_ptr<KdRenderTexture> m_color;
	// �@��
	std::shared_ptr<KdRenderTexture> m_normal;

	// m_color����͗p�ɃR�s�[��������
	std::shared_ptr<KdRenderTexture> m_inputColor;
	// m_normal����͗p�ɃR�s�[��������
	std::shared_ptr<KdRenderTexture> m_inputNormal;

	// ���s���p�V���h�E�}�b�v
	std::shared_ptr<KdRenderTexture> m_dirLightShadowMap;
};
