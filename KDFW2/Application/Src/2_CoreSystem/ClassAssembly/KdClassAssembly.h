#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

//================================================
// 
// クラス登録
// ・ここにクラスを登録することで、クラス名(文字列)からインスタンスを生成することが可能になる
// 
//================================================
class KdClassAssembly
{
public:

	// クラス名からクラスインスタンスを生成する(shared_ptr)
	template<class T>
	static std::shared_ptr<T> MakeShared(const std::string& className)
	{
		auto it = s_assemblies->find(className);
		if (it != s_assemblies->end())
		{
			// インスタンスを生成する
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

	// クラスマップ
	// key:クラス名
	// value:インスタンスを生成する関数
	static std::unordered_map<std::string, Node>* s_assemblies;

public:
	// クラス登録用クラス
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

			// クラス名をキーに、クラス生成関数を登録する
			Node node;
			node.Tag = tag;
			node.CreateProc = CreateInstance;
			(*s_assemblies)[className] = node;
		}
		// インスタンス生成関数
		static void* CreateInstance() { return new T(); }
	};
};

// クラス登録マクロ(cppに書く)
#define SetClassAssembly(className, tag) \
namespace className##Assembly \
{ \
	static KdClassAssembly::Reflection<className> reflection(#className, tag); \
};
