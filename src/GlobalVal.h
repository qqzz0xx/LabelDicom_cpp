#pragma once

#include <string>
#include <QString>
#include "json.hpp"
#include <QTemporaryDir>

struct GlobalVal
{
	bool IsPrjectChanged = false;

	QString FileName;
	double CameraZoomFactor = 1.0;
	double PolyActorZoomFactor = 1.0;
	bool IsMaskChanged = false;
	bool IsInRandomSlice = false;
	QTemporaryDir TempDir;

};

struct ConfigVal
{
	QString AES_KEY = "RayShape1212";
	bool CanRepeatLabel = false;
	int PlayInterval = 100; //ms
	bool IsLoadDicomToVideo = false;
	std::vector<std::pair<std::string, std::string>> SegModelPaths;

	void Load(const nlohmann::json& j);
	void Save(nlohmann::json& j);
};

extern GlobalVal g_GlobalVal;
extern ConfigVal g_ConfigVal;