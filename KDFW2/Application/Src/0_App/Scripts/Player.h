#pragma once

class Collision;

//�v���C���[�𐧌䂷��R���|�[�l���g
class Player :public KdScriptComponent
{
public:
	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;


	//Player�̏��𒲐�����
	virtual void Editor_ImGui()override;

	// ���̃N���X�̓��e��JSON�f�[�^������(�ۑ�)
	virtual void Serialize(nlohmann::json& outJson) const override;
	

	// JSON�f�[�^����A�N���X�̓��e��ݒ�i�ǂݍ��݁j
	virtual void Deserialize(const nlohmann::json& jsonObj) override;
	

private:

	// ����Player����ɏ���Ă���I�u�W�F�N�g
	KdGameObject* m_ridingObject = nullptr;

	float m_speed=1.0f;
	
	//�L�����N�^�[�̈��̈ړ���
	KdVector3 m_vMoveOnce = {};


	std::shared_ptr<Collision> m_spCollision = nullptr;

	//�A�j���[�V�����֌W
	std::shared_ptr<KdAnimationComponent> m_animation = nullptr;

};