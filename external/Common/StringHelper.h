#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace StringHelper
{
	void StringReplace(std::string &strBig, const std::string &strsrc, const std::string &strdst);
	std::vector<std::string> StringSplit(const std::string &strBig, const std::string &strdst);

	//�������·��
	std::string CompRelativePath(const std::string& root, const std::string& path);
}
