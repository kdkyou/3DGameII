#pragma once

#include <string>

#pragma comment(lib ,"rpcrt4.lib")

class KdGuid
{
public:

	KdGuid()
	{
		NewGuid();
	}

	// 新しいGUIDを作成する
	void NewGuid()
	{
		// すでにGuidが設定されているかどうか
		std::string prevId = ToString();
		// Guid発行
		UuidCreate(&m_guid);

		if (prevId != "")
		{
			// Guidが振り直された
			// 以前のGuidをキーにして新しいGuidを要素にする
			m_replacedGuids[prevId] = ToString();
		}
	}

	std::string ToString() const
	{
		std::string ret;
		RPC_CSTR String;
		if (UuidToStringA(&m_guid, &String) == RPC_S_OK)
		{
			ret = (char*)String;
		}
		return ret;
	}

	void FromString(const std::string& strGuid)
	{
		UuidFromStringA((RPC_CSTR)strGuid.c_str(), &m_guid);
	}

	/*
	static KdGuid sNullGuid()
	{
		static UUID nullGuid;
		UuidCreateNil(&nullGuid);
	}
	*/

	// 置き換えられた新しいGUIDを古いGUIDから探す
	static const std::string& GetRePlacedGuid(
		const std::string& oldGuid)
	{
		// 新しく振り直されたかどうかを探す
		if (m_replacedGuids.find(oldGuid) != m_replacedGuids.end())
		{
			return m_replacedGuids[oldGuid];
		}
		// 見つからなかった
		return oldGuid;
	}

private:
	UUID m_guid = {};

	// 置き換えられたGuid一覧
	static std::map<std::string, std::string> m_replacedGuids;
	// ↑　KdGuidクラスが複数インスタンス化されても
	//　一つしかメモリ確保されない変数 = staticメンバ変数
	// static メンバ関数 staticなメンバしかアクセスできない
	
	// static const float KOTE;
};