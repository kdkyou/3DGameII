#pragma once

class CheckList : public KdScriptComponent
{
public:

	void Start()		override;
	void Update()		override;
	void LateUpdate()	override;
	virtual void Serialize(nlohmann::json& outJson) const override;			// (�ۑ�)
	virtual void Deserialize(const nlohmann::json& jsonObj) override;		// �i�ǂݍ��݁j
	virtual void Editor_ImGui()override;													//�`�F�b�N���X�g�p

	const bool IsGround() { return m_isGround; }
	const bool IsEnemy() { return m_isEnemy; }
	const bool IsPlayer() { return m_isPlayer; }

	//�O������Ăяo���p
	const bool IsDirty();

private:

	bool m_isGround = false;		//�n�`
	bool  m_isEnemy = false;		//�G
	bool m_isPlayer = false;			//�v���C���[
	
	bool m_isDirty = false;			//�t���O��؂�ւ������ǂ���

};