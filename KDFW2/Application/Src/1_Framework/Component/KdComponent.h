#pragma once

class KdGameObject;

//=====================================================
//
// �R���|�[�l���g��{�N���X
//
//=====================================================
class KdComponent : public KdObject
{
public:

	// ������ݒ�
	void SetGameObject(std::shared_ptr<KdGameObject> obj) { m_pGameObject = obj; }
	// ������擾
	std::shared_ptr<KdGameObject> GetGameObject() const { return m_pGameObject.lock(); }
	// �����傪���݂��邩
//	bool ExistOwner() const { return m_pGameObject.expired() == false; }

	// �L���t���O�ݒ�
	void SetEnable(bool enable) { m_enable = enable; }
	// �L���t���O�擾
	bool IsEnable() const { return m_enable; }

	// ����ɂP�x�������s�����֐�
	virtual void Start() {}

	// �X�V����
	virtual void Update() {}
	// �X�V����
	virtual void LateUpdate() {}

	// �`�揀������
	virtual void PreDraw() {}

	// �`�揈��
	virtual void Draw(int phaseID) {}


	
	// ����̂�Start�֐����Ăяo��
	void CallStartOnce()
	{
		// ����t���O��On�̎��̂�
		if (m_isNew)
		{
			Start();		// Start���s
			m_isNew = false;
		}
	}
	

	//===============================
	// Serialize / Deserialize
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	virtual void Deserialize(const nlohmann::json& jsonData)
	{
		m_enable = jsonData["Enable"].get<bool>();
	}

	// ���̃N���X�̓��e��JSON�f�[�^������
	virtual void Serialize(nlohmann::json& outJson) const
	{
		outJson["ClassName"] = GetClassName();
		outJson["Enable"] = m_enable;
	}

	//===============================
	// Editor
	//===============================
	virtual void Editor_ImGui();

	//===============================
	// ���̑�
	//===============================

protected:

	// �L��/�����t���O
	bool		m_enable = true;

	// �쐬�����āH�̃t���O
	bool		m_isNew = true;

	// ������ւ̃A�h���X
	std::weak_ptr<KdGameObject> m_pGameObject;
};

// �`�悪�K�v�ȃR���|�[�l���g
class KdRendererComponent : public KdComponent
{
public:
	virtual void Draw(bool opaque, KdShader::PassTags passTag) {}
};