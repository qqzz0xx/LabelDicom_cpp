#include "GlobalVal.h"
#include "JsonHelper.h"

GlobalVal g_GlobalVal;
ConfigVal g_ConfigVal;

void ConfigVal::Load(const nlohmann::json& j)
{
	if (j.is_null()) return;

	CanRepeatLabel = j["CanRepeatLabel"];
	JsonHelper::TryGetValue(j, "IsLoadDicomToVideo", IsLoadDicomToVideo);

	nlohmann::json j_SegModelPaths;
	JsonHelper::TryGetValue(j, "SegModelPaths", j_SegModelPaths);

	SegModelPaths.clear();
	for (auto&& v : j_SegModelPaths)
	{
		auto p = std::make_pair((std::string)v["Name"], (std::string)v["Path"]);
		SegModelPaths.emplace_back(p);
	}
}

void ConfigVal::Save(nlohmann::json& j)
{
	j["CanRepeatLabel"] = CanRepeatLabel;
	j["IsLoadDicomToVideo"] = IsLoadDicomToVideo;

	nlohmann::json j_s;
	for (auto&& v : SegModelPaths)
	{
		nlohmann::json t;
		t["Name"] = v.first;
		t["Path"] = v.second;

		j_s.push_back(j_s);
	}
	j["SegModelPaths"] = j_s;
}
