#include "StringHelper.h"
#include <algorithm>

void StringHelper::StringReplace(std::string &strBig, const std::string &strsrc, const std::string &strdst)
{
	std::string::size_type pos = 0;
	std::string::size_type srclen = strsrc.size();
	std::string::size_type dstlen = strdst.size();

	while ((pos = strBig.find(strsrc, pos)) != std::string::npos)
	{
		strBig.replace(pos, srclen, strdst);
		pos += dstlen;
	}
}

std::vector<std::string> StringHelper::StringSplit(const std::string &strBig, const std::string &strdst)
{
	std::string::size_type splitLen = strdst.length();
	std::string::size_type start = 0;
	std::string::size_type pos = 0;
	std::vector<std::string> ss;

	while ((pos = strBig.find(strdst, start)) != std::string::npos)
	{
		std::string s(strBig.begin() + start, strBig.begin() + pos);
		ss.push_back(s);
		start = pos + splitLen;
	}

	if (start != strBig.size())
	{
		std::string s(strBig.begin() + start, strBig.end());
		ss.push_back(s);
	}

	return ss;
}

std::string StringHelper::CompRelativePath(const std::string& root, const std::string& path)
{
	std::string root_ = root;
	std::string path_ = path;

	std::transform(root_.begin(), root_.end(), root_.begin(), ::tolower);
	std::transform(path_.begin(), path_.end(), path_.begin(), ::tolower);

	StringReplace(root_, "\\", "/");
	StringReplace(path_, "\\", "/");

	auto rootSplit = StringSplit(root_, "/");
	auto pathSplit = StringSplit(path_, "/");

	for (int i = 0; i < rootSplit.size() && i < pathSplit.size(); i++)
	{
		if (rootSplit[i] != pathSplit[i])
		{
			std::stringstream ss;
			for (int j = i; j < rootSplit.size(); j++)
			{
				ss << "../";
			}

			for (int j = i; j < pathSplit.size(); j++)
			{
				ss << pathSplit[j] << "/";
			}

			auto str = ss.str();

			return str.substr(0, str.size() - 1);
		}
	}

	return std::string();
}