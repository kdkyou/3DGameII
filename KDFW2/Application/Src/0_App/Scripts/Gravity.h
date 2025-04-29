#pragma once

class Player;

class Gravity :public KdScriptComponent
{
public:
	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;
	

	//Gravity�̏��𒲐�����
	virtual void Editor_ImGui()override;

	// ���̃N���X�̓��e��JSON�f�[�^������(�ۑ�)
	virtual void Serialize(nlohmann::json& outJson) const override;


	// JSON�f�[�^����A�N���X�̓��e��ݒ�i�ǂݍ��݁j
	virtual void Deserialize(const nlohmann::json& jsonObj) override;


private:

	float m_gravity = 0.915f;	//�d��
	float m_fall = 0.0f;		//���Z���ꂽ�ړ���

	float m_jumpPow = 2.0f;		//�W�����v��

	float m_intervalVal = 1.0f;	//�����̃C���^�[�o���̒l
	float m_interval = 0.0f;	//�����̃C���^�[�o��

	//�L�[���͂��邩������
	int m_isSelect = 0;

	enum Select
	{
		None,
		Key,
		Auto,
	};
};