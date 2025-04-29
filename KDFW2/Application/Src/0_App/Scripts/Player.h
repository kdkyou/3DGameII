#pragma once

//�v���C���[�𐧌䂷��R���|�[�l���g

class Player :public KdScriptComponent
{
public:
	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;

	//�����蔻��֐�
	void CollisionPhase(); 


	//Player�̏��𒲐�����
	virtual void Editor_ImGui()override;

	// ���̃N���X�̓��e��JSON�f�[�^������(�ۑ�)
	virtual void Serialize(nlohmann::json& outJson) const override;
	

	// JSON�f�[�^����A�N���X�̓��e��ݒ�i�ǂݍ��݁j
	virtual void Deserialize(const nlohmann::json& jsonObj) override;
	

private:

	float m_speed=1.0f;
	float m_fall = 0.0f;

	//�L�����N�^�[�̈��̈ړ���
	KdVector3 m_vMoveOnce = {};

	float m_jumpPow = 10.0f;

	float m_gravity = 0.915f;


	//�����Œǉ������ϐ�
	float m_rayCorrection = 0.2f;
	float m_sphereCorrection = 0.5f;
	float m_sphereRadius = 0.3f;

	//�A�j���[�V�����֌W
	std::shared_ptr<KdAnimationComponent> m_animation = nullptr;

};