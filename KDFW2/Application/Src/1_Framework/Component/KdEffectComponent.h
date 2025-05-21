#pragma once

#include"KdComponent.h"

class KdPolygon;	//�O���錾

//�G�t�F�N�g�Ǘ��R���|�[�l���g
class KdEffectComponent : public KdRendererComponent
{
public:
	
	virtual void Start() override;
	virtual void Update()override;
	virtual void LateUpdate()override;
	virtual void PreDraw()override;
	virtual void Draw(bool opaque,KdShader::PassTags passTag)override;

	virtual void Editor_ImGui()override;

private:

	//�G�t�F�N�g��`�悷��|���S���Ǘ�
	std::shared_ptr<KdPolygon> m_poly = nullptr;
	//�G�t�F�N�g�e�N�X�`��
	std::shared_ptr<KdTexture> m_texture = nullptr;

};