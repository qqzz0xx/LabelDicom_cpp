#pragma once
#include "LocalConfig.h"
#include <string>

class StringMgr
{
private:
	StringMgr() 
	{
		m_Config.SetFileName("String.json");
		m_Config.Load();
	}
	StringMgr(const StringMgr&);


	LocalConfig m_Config;

public:

	static StringMgr * Inst()
	{
		static StringMgr inst;
		return &inst;
	}

	std::string GetString(const std::string& key)
	{
		 auto str = m_Config.GetValue<std::string>(key);
		 return str;
	}

};

