#pragma once

#include "json.hpp"

namespace KdJsonUtility
{
	//=================================================
	// 
	// 取得系
	// 
	//=================================================

	// baseJsonObjに、指定したメンバ(key)が存在する場合、それを取得する。ない場合は取得されない。
	template<class Type>
	static void GetValue(const nlohmann::json& baseJsonObj, const std::string& key, Type* out)
	{
		auto it = baseJsonObj.find(key);
		if (it == baseJsonObj.end()) return;

		*out = (*it).get<Type>();
	}

	// baseJsonObjに、指定したメンバ(key)が存在する場合、それを取得しprocを実行する。
	static void GetValue(const nlohmann::json& baseJsonObj, const std::string& key, std::function<void(nlohmann::json)> proc)
	{
		auto it = baseJsonObj.find(key);
		if (it == baseJsonObj.end()) return;

		proc(*it);
	}

	// baseJsonObjに、指定した配列メンバ(key)が存在する場合、それを取得する。ない場合は取得されない。
	template<class Type>
	static void GetArray(const nlohmann::json& baseJsonObj, const std::string& key, Type* out, uint32_t arrayCount)
	{
		auto it = baseJsonObj.find(key);
		if (it == baseJsonObj.end()) return;

		for (uint32_t i = 0; i < arrayCount; i++)
		{
			out[i] = it->at(i).get<Type>();
		}
	}

	// baseJsonObjに、指定した配列メンバ(key)が存在する場合、それを取得しoutに「追加」する。ない場合は取得されない。
	template<class Type>
	static void GetArray(const nlohmann::json& baseJsonObj, const std::string& key, std::vector<Type>& out)
	{
		auto it = baseJsonObj.find(key);
		if (it == baseJsonObj.end()) return;

		for (uint32_t i = 0; i < it->size(); i++)
		{
			out.push_back(it->at(i).get<Type>());
		}
	}

	// baseJsonObjに、指定した配列メンバ(key)が存在する場合、それを取得しprocを実行する。
	static void GetArray(const nlohmann::json& baseJsonObj, const std::string& key, std::function<void(uint32_t, nlohmann::json)> proc)
	{
		auto it = baseJsonObj.find(key);
		if (it == baseJsonObj.end()) return;

		for (uint32_t i = 0; i < it->size(); i++)
		{
			proc(i, it->at(i));
		}
	}

	//=================================================
	// 
	// 作成系
	// 
	//=================================================

	// Jsonの配列データを作成する
	template<class Type>
	static nlohmann::json CreateArray(Type* src, uint32_t arrayCount)
	{
		auto ary = nlohmann::json::array();

		for (uint32_t i = 0; i < arrayCount; i++)
		{
			ary.push_back(src[i]);
		}
		return ary;
	}
};
