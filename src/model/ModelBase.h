#pragma once
#include "Header.h"
#include "json.hpp"

class SliceView;
class ModelBase
{
public:
	ModelBase();
	std::string GetName() { return m_Name; }
	virtual void OnFrameChanged(SliceView* view, int value) {}
	virtual void Clear() {}
	virtual void Exit() {}
	virtual void Start() {}
	virtual void ImportJson(const nlohmann::json& json) {}
	virtual void ExportJson(nlohmann::json& json) {}
protected:
	std::string m_Name;
};

