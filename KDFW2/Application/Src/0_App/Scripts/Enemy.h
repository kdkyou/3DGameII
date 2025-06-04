#pragma once

class Collision;

class Enemy :public KdScriptComponent
{
public:

	virtual void Start() override;
	virtual void Update()override;
	virtual void LateUpdate()	override;


	// ���̃N���X�̓��e��JSON�f�[�^������(�ۑ�)
	virtual void Serialize(nlohmann::json& outJson) const override;

	// JSON�f�[�^����A�N���X�̓��e��ݒ�i�ǂݍ��݁j
	virtual void Deserialize(const nlohmann::json& jsonObj) override;

	//�G�l�~�[�̏��𒲐�����
	virtual void Editor_ImGui()override;

private:

	float m_speed = 2.0f;			//�ړ����x
	float m_ang = 3.0f;			//��]�p�x
	float m_difference = 3.0f;		//�ێ�����

	//Collision�R���|�[�l���g
	std::shared_ptr<Collision>	m_spCollision = nullptr;

};