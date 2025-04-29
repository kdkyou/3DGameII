#pragma once


#include "Component/KdTransformComponent.h"

//=============================================================
// 
// GameObject
// 
//=============================================================
class KdGameObject final : public KdObject
{
public:

	KdGameObject() {}

	// ���O�ݒ�
	void SetName(const std::string& name) { m_name = name; }
	// ���O�擾
	const std::string& GetName() const { return m_name; }

	// �L���t���O�ݒ�
	void SetEnable(bool enable) { m_enable = enable; }
	// �L���t���O�擾
	bool IsEnable() const { return m_enable; }

	// Tag�ݒ�
	void SetTag(const std::string& tag){m_tag = tag;}
	const std::string& GetTag() const {return m_tag;}

	// Transform�擾
	std::shared_ptr<KdTransformComponent> GetTransform() const { return std::static_pointer_cast<KdTransformComponent>(m_components[0]); }

	// �����⎩���̎q�����ΏۂɁA�L����GameObject��gameObjectList�֓o�^����
	void Collect(std::vector<KdGameObject*>& gameObjectList);

	// �����ݒ�
	void Initialize(const std::string& name);

	//===============================
	//
	// �R���|�[�l���g
	//
	//===============================

	// �R���|�[�l���g���X�g�擾
	const std::vector<std::shared_ptr<KdComponent>>& GetComponentList() const { return m_components; }

	// �R���|�[�l���g�ǉ�
	template<class T>
	std::shared_ptr<T> AddComponent()
	{
		auto comp = std::make_shared<T>();
		AddComponent(comp);
		return comp;
	}
	// �R���|�[�l���g�ǉ�(�N���X�����w��)
	std::shared_ptr<KdComponent>AddComponent(const std::string& className);

	// �R���|�[�l���g�ǉ�
	void AddComponent(const std::shared_ptr<KdComponent>& comp);

	// �w��^�̃R���|�[�l���g�擾
	template<class T>
	std::shared_ptr<T> GetComponent()
	{
		// ����
		for (auto&& comp : m_components)
		{
			// �_�E���L���X�g�Ō^����
			auto p = std::dynamic_pointer_cast<T>(comp);
			if (p)return p;
		}
		return nullptr;
	}

	// �w��^�̃R���|�[�l���g�擾(�q���܂�)
	template<class T>
	std::shared_ptr<T> GetComponentInChildren()
	{
		auto comp = GetComponent<T>();
		if (comp != nullptr) return comp;

		// �q����������
		for (auto&& child : m_children)
		{
			comp = child->GetComponentInChildren<T>();
			if (comp != nullptr) return comp;
		}

		return nullptr;
	}

	// �w��^�̃R���|�[�l���g��S�Ď擾
	// �Eresult		�c ���������R���|�[�l���g���S�Ă����ɓ��� 
	// �EwithChild	�c �qGameObject����������H
	template<class T>
	void GetComponents(std::vector<std::shared_ptr<T>>& result)
	{
		// ����
		for (auto&& comp : m_components)
		{
			// �_�E���L���X�g�Ō^����
			auto p = std::dynamic_pointer_cast<T>(comp);
			if (p != nullptr) 
				result.push_back(p);
		}
	}

	// �w��^�̃R���|�[�l���g��S�Ď擾
	// �Eresult		�c ���������R���|�[�l���g���S�Ă����ɓ��� 
	// �EwithChild	�c �qGameObject����������H
	template<class T>
	void GetComponentsInChildren(std::vector<std::shared_ptr<T>>& result)
	{
		// ����
		for (auto&& comp : m_components)
		{
			// �_�E���L���X�g�Ō^����
			auto p = std::dynamic_pointer_cast<T>(comp);
			if (p != nullptr)
				result.push_back(p);
		}
		// �q����������
		for (auto&& child : m_children)
		{
			child->GetComponentsInChildren(result);
		}
	}

	// �w��^�̃R���|�[�l���g�擾(�e�������܂�)
	template<class T>
	std::shared_ptr<T> GetComponentInParent()
	{
		auto comp = GetComponent<T>();
		if (comp != nullptr) return comp;

		auto parent = m_parent.lock();
		if (parent == nullptr) return nullptr;

		return parent->GetComponentInParent<T>();
	}

	//===============================
	// 
	// �K�w�\��
	// 
	//===============================

	// �e�擾
	std::shared_ptr<KdGameObject> GetParent() const { return m_parent.lock(); }
	void SetParent(const std::shared_ptr<KdGameObject>& newParent, bool worldPositionStays);

	// �q���X�g�擾
	const std::list<std::shared_ptr<KdGameObject>>& GetChildren() { return m_children; }

	//===============================
	// 
	// Serialize / Deserialize
	// 
	//===============================
	// JSON�f�[�^����A�N���X�̓��e��ݒ�
	void Deserialize(const nlohmann::json& jsonObj);
	// ���̃N���X�̓��e��JSON�f�[�^������
	void Serialize(nlohmann::json& outJson) const;


	//===============================
	// Editor
	//===============================
	void Editor_ImGui();

private:

	// �ŗLID
	KdGuid									m_guid;

	// ���O
	std::string								m_name = "GameObject";

	// �L���t���O
	bool									m_enable = true;

	// Tag
	std::string								m_tag = "";

	// �R���|�[�l���g���X�g
	std::vector<std::shared_ptr<KdComponent>>		m_components;

	// �q���X�g
	std::list<std::shared_ptr<KdGameObject>>		m_children;
	// �e�̃|�C���^
	std::weak_ptr<KdGameObject>						m_parent;
};
