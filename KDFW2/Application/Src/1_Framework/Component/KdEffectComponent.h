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

	//�G�f�B�^���
	virtual void Editor_ImGui()override;

	//�ۑ��Ǎ�
	virtual void Deserialize(const nlohmann::json& jsonObj)override;
	virtual void Serialize(nlohmann::json& outJson) const override;

private:

	//����ް�މ�������̒ǉ���Z��]
	float m_zRotation = 0.0f;

	KdVector2 m_scale = { 1.0f,1.0f };


	//�G�t�F�N�g��`�悷��|���S���Ǘ�
	std::shared_ptr<KdPolygon> m_poly = nullptr;
	//�G�t�F�N�g�e�N�X�`��
	std::shared_ptr<KdTexture> m_texture = nullptr;

};