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

	KdVector3 m_color = { 1.0f,1.0f,1.0f };

	//�A�j���[�V��������	�ۑ�
	// m_lifetime�̎��Ԃ�������
	// m_startScalee����m_endScale �܂Ŋg�k��ύX(���`���)
	// �A�j���[�V�����͕ۑ��ł���悤�ɂ���
	
	//�G�t�F�N�g�̃A�j���[�V��������
	float m_lifetime = 3.0f;	
	//�J�n���̊g�k��
	KdVector2 m_startScale = {1,1};
	//�I�����̊g�k��
	KdVector2 m_endScale = {3,5};

	// Start�Ɠ����Ɋg�k�����邩�ǂ���
	bool m_playWithStart = false;	


	// �A�j���[�V�������J�n������
	bool m_isStarted = false;
	//�J�n���Ă���̌o�ߎ���
	float m_duration = 0.0f;

	KdEase			m_ease;

	void StartAnim();

	//�G�t�F�N�g��`�悷��|���S���Ǘ�
	std::shared_ptr<KdPolygon> m_poly = nullptr;
	//�G�t�F�N�g�e�N�X�`��
	std::shared_ptr<KdTexture> m_texture = nullptr;

	//�g�p����e�N�X�`����path
	std::string m_textureFilePath = "";

};