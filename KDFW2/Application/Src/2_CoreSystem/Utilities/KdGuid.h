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

	// V‚µ‚¢GUID‚ğì¬‚·‚é
	void NewGuid()
	{
		UuidCreate(&m_guid);
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

private:
	UUID m_guid = {};
};