#include "LocalConfig.h"

#include <fstream>
#include <iostream>
#include <windows.h>
#include <filesystem>

namespace fs = std::filesystem;

LocalConfig::LocalConfig()
{
}

LocalConfig::~LocalConfig()
{
	Save();
}

void LocalConfig::SetFileName(const std::string & fileName)
{
	m_FileName = fileName;
}

bool LocalConfig::Load()
{
	if (!m_FileName.empty() )
	{
		auto root = GetRootPath();
		auto full = root + "/" + m_FileName;

		if (!fs::exists(full)) return false;

		std::ifstream ifs(full);
		try
		{
			ifs >> m_JsonData;
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}

		return true;
	}

	return false;
}

void LocalConfig::Save()
{
	if (!m_FileName.empty() && !m_JsonData.is_null())
	{
		auto root = GetRootPath();

		std::ofstream o(root + "/" + m_FileName);
		try
		{
			o << std::setw(4) << m_JsonData << std::endl;
		}
		catch (std::exception & ex)
		{
			std::cout << ex.what() << std::endl;
		}

	}
}

void LocalConfig::SetRootPath(const std::string& path)
{
	m_RootPath = path;
}

const std::string& LocalConfig::GetRootPath()
{
	if (m_RootPath.empty())
	{
		m_RootPath = GetAppRootPath();
	}

	return m_RootPath;
}

LocalConfig * LocalConfig::GetGlobalConfig()
{
	static LocalConfig s_config;
	s_config.SetFileName("GlobalConfig.json");
	s_config.Load();

	return &s_config;
}

std::string LocalConfig::GetAppRootPath()
{
	char chpath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, chpath, MAX_PATH);
	std::string tmpPath(chpath);
	int pos = tmpPath.find_last_of('\\', tmpPath.length());
	tmpPath = tmpPath.substr(0, pos);

	return tmpPath;
}
