#pragma once

#include "json.hpp"

#include <string>

using namespace nlohmann;

class LocalConfig
{
public:
	LocalConfig();
	~LocalConfig();

	template<class T>
	T GetValue(const std::string& key);

	template<class T>
	void SetValue(const std::string& key, const T& value);

	void SetJsonData(const nlohmann::json & j) { m_JsonData = j; }

	nlohmann::json GetJsonData() { return m_JsonData; };

	void SetFileName(const std::string& fileName);

	bool Load();

	void Save();

	void SetRootPath(const std::string& path);
	const std::string& GetRootPath(); 

	static LocalConfig* GetGlobalConfig();

	static std::string GetAppRootPath();

private:
	json m_JsonData;
	std::string m_FileName;
	std::string m_RootPath;
};

template<class T>
inline T LocalConfig::GetValue(const std::string & key)
{
	if ( !m_JsonData.is_null() && !m_JsonData[key].is_null())
	{
		return m_JsonData[key];
	}
	return T();
}

template<class T>
inline void LocalConfig::SetValue(const std::string & key, const T & value)
{
	m_JsonData[key] = value;
}
