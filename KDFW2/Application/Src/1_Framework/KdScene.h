#pragma once

#include "Component/KdModelRendererComponent.h"

#include "KdGameObject.h"

//=========================================================
// 
// �V�[��
// Hierarchy���Ǘ����Ă���N���X
// 
//=========================================================
class KdScene : public KdObject
{
public:
	//�v���n�u�t�@�C������GameObject�𓮓I����
	//�߂�l�F�V�����쐬����GameObject
	//prefabFilePat�F�Q�l�ɂ���v���n�u�t�@�C��
	//parent�F�V�����������ꂽGameObject�̐e
	//		  �w�肪�Ȃ�������Root�����ɍ��
	std::shared_ptr<KdGameObject>	Instantiate(
		const std::string& prefabFilePath,
		std::shared_ptr<KdGameObject> parent = nullptr
	);

	// Root GameObject���擾
	std::shared_ptr<KdGameObject> GetRootObject() { return m_rootObject; }

	// CollectGameObjects()�Ŏ��W����GameObject���X�g���擾
	const std::vector<KdGameObject*>& GetCollectedObjects() const { return m_collectedObjects; }

	// �����ݒ�
	void Initialize()
	{
		m_rootObject = std::make_shared<KdGameObject>();
		m_rootObject->Initialize("Root");
	}

	// 
	void ResetFrameData()
	{
		m_collectedObjects.clear();
	}

	// �SGameObject����A�L���Ȃ��̂�S�ďW�߂�
	void CollectGameObjects()
	{
		m_rootObject->Collect(m_collectedObjects);
	}

	// 
	void Update()
	{
		// ���W����GameObject��Update�����s����
		for (auto&& gameObj : m_collectedObjects)
		{
			//�V���ɓo�^���ꂽ�I�u�W�F�N�g��Start���Ăяo���Ă���
			for (auto&& comp : gameObj->GetComponentList())
			{
				comp->CallStartOnce();
			}

			for (auto&& comp : gameObj->GetComponentList())
			{
				comp->Update();
			}
		}
	}

	void Release()
	{
		m_rootObject = nullptr;

		m_collectedObjects.clear();
	}

	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	void Deserialize(const nlohmann::json& jsonObj);
	// ���̃N���X�̓��e��JSON�f�[�^������
	void Serialize(nlohmann::json& outJson) const;

	// �t�@�C������ǂݍ���
	bool LoadFromFile(const std::string& filepath);

	// �t�@�C���֕ۑ�
	bool SaveToFile(const std::string& filepath);

	//===============================
	// Editor
	//===============================
	void Editor_ImGui();

private:

	// Top��GameObject
	std::shared_ptr<KdGameObject>			m_rootObject;

	// ���W���ꂽGameObject�̃��X�g
	std::vector<KdGameObject*>				m_collectedObjects;

};
