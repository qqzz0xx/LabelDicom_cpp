#pragma once

#include "Header.h"
#include "ModelBase.h"
#include "Types.h"
#include "Structs.h"
#include "ColorLabelTableModel.h"
#include "TextActor2D.h"

class SliceView;


enum FrameLabelType
{
	FrameLabelType_Frame,
	FrameLabelType_Struct,
	FrameLabelType_Illness,
};
struct FrameLabelItem
{
	MprType ViewType;
	int FrameCount;
	std::shared_ptr<ColorLabelItem> LabelType;
	SliceView* View;
	FrameLabelType ItemType;
};

class FrameLabelModel : public ModelBase
{
public:
	FrameLabelModel();
	void Add(std::shared_ptr<FrameLabelItem> p);
	void Remove(SliceView* view, int FrameCount);

	virtual void Clear() override;
	virtual void OnFrameChanged(SliceView* view, int value) override;
	virtual void ExportJson(nlohmann::json& j) override;
	virtual void ImportJson(const nlohmann::json& json) override;
	void ForEach(std::function<void(std::shared_ptr<FrameLabelItem>)> func);

private:
	std::vector<std::shared_ptr<FrameLabelItem>> m_FrameLabels;
	std::string m_FrameTexts[4];

	std::shared_ptr<FrameLabelItem> m_StructLabelItem;
	std::shared_ptr<FrameLabelItem> m_IllnessLabelItem;
	//std::map<int, std::weak_ptr<NRender::TextActor2D>> m_TextActorMap;
	
};

