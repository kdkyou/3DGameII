#pragma once

#include "json.hpp"

namespace KdJsonUtility
{
	//=================================================
	// 
	// �擾�n
	// 
	//=================================================

	// baseJsonObj�ɁA�w�肵�������o(key)�����݂���ꍇ�A������擾����B�Ȃ��ꍇ�͎擾����Ȃ��B
	template<class Type>
	static void GetValue(const nlohmann::json& baseJsonObj, const std::string& key, Type* out)
	{
		auto it = baseJsonObj.find(key);
		if (it == baseJsonObj.end()) return;

		*out = (*it).get<Type>();
	}

	// baseJsonObj�ɁA�w�肵�������o(key)�����݂���ꍇ�A������擾��proc�����s����B
	static void GetValue(const nlohmann::json& baseJsonObj, const std::string& key, std::function<void(nlohmann::json)> proc)
	{
		auto it = baseJsonObj.find(key);
		if (it == baseJsonObj.end()) return;

		proc(*it);
	}

	// baseJsonObj�ɁA�w�肵���z�񃁃��o(key)�����݂���ꍇ�A������擾����B�Ȃ��ꍇ�͎擾����Ȃ��B
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

	// baseJsonObj�ɁA�w�肵���z�񃁃��o(key)�����݂���ꍇ�A������擾��out�Ɂu�ǉ��v����B�Ȃ��ꍇ�͎擾����Ȃ��B
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

	// baseJsonObj�ɁA�w�肵���z�񃁃��o(key)�����݂���ꍇ�A������擾��proc�����s����B
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
	// �쐬�n
	// 
	//=================================================

	// Json�̔z��f�[�^���쐬����
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
