#pragma once

/*
#include "Component/KdModelRendererComponent.h"

#include "KdGameObject.h"

//=========================================================
// 
// GameObject���Ǘ�����N���X
// 
//=========================================================
class KdGameObjectCollection
{
public:

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

	// Editor
	void Editor_ImGui();

private:

	// Top��GameObject
	std::shared_ptr<KdGameObject>			m_rootObject;

	// ���W���ꂽGameObject�̃��X�g
	std::vector<KdGameObject*>				m_collectedObjects;
};
*/