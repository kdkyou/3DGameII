#pragma once

class Player;

class Gravity :public KdScriptComponent
{
public:
	virtual void Start()		override;
	virtual void Update()		override;
	virtual void LateUpdate()	override;
	

	//Gravity�̏��𒲐�����
	virtual void Editor_ImGui()override;

	// ���̃N���X�̓��e��JSON�f�[�^������(�ۑ�)
	virtual void Serialize(nlohmann::json& outJson) const override;


	// JSON�f�[�^����A�N���X�̓��e��ݒ�i�ǂݍ��݁j
	virtual void Deserialize(const nlohmann::json& jsonObj) override;

	const KdVector3& GetMoveOnes() const { return m_vMoveOnes; }

	void Ground() { m_isGround = true; }

private:

	//���n����t���O
	bool m_isGround = false;

	float m_gravity = 0.915f;	//�d��
	float m_fall = 0.0f;		//���Z���ꂽ�ړ���
	KdVector3 m_vMoveOnes = { 0,0,0 };

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