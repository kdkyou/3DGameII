#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

//================================================
// 
// �N���X�o�^
// �E�����ɃN���X��o�^���邱�ƂŁA�N���X��(������)����C���X�^���X�𐶐����邱�Ƃ��\�ɂȂ�
// 
//================================================
class KdClassAssembly
{
public:

	// �N���X������N���X�C���X�^���X�𐶐�����(shared_ptr)
	template<class T>
	static std::shared_ptr<T> MakeShared(const std::string& className)
	{
		auto it = s_assemblies->find(className);
		if (it != s_assemblies->end())
		{
			// �C���X�^���X�𐶐�����
			return std::shared_ptr<T>((T*)it->second.CreateProc());
		}
		return nullptr;
	}

public:

	struct Node
	{
		std::string Tag;
		std::function<void* ()> CreateProc;
	};

	// �N���X�}�b�v
	// key:�N���X��
	// value:�C���X�^���X�𐶐�����֐�
	static std::unordered_map<std::string, Node>* s_assemblies;

public:
	// �N���X�o�^�p�N���X
	template<class T>
	class Reflection
	{
	public:
		Reflection(const std::string& className, const std::string& tag)
		{
			if (s_assemblies == nullptr)
			{
				s_assemblies = new std::unordered_map<std::string, Node>();
			}

			// �N���X�����L�[�ɁA�N���X�����֐���o�^����
			Node node;
			node.Tag = tag;
			node.CreateProc = CreateInstance;
			(*s_assemblies)[className] = node;
		}
		// �C���X�^���X�����֐�
		static void* CreateInstance() { return new T(); }
	};
};

// �N���X�o�^�}�N��(cpp�ɏ���)
#define SetClassAssembly(className, tag) \
namespace className##Assembly \
{ \
	static KdClassAssembly::Reflection<className> reflection(#className, tag); \
};
