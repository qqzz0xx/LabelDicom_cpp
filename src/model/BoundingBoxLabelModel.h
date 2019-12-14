#pragma once
#include "Header.h"
#include "ModelBase.h"
#include "Types.h"
#include "Structs.h"
#include "ColorLabelTableModel.h"

namespace NRender
{
	class BoundingBoxActor;
}

struct BoundingBoxLabelItem : public UObject
{
	MprType SliceType;
	int FrameCount;
	glm::dvec3 Bounding[2];
	std::shared_ptr<ColorLabelItem> LabelType;
};

class BoundingBoxLabelModel : public ModelBase
{
public:
	BoundingBoxLabelModel();
	void Add(SliceView* view, std::shared_ptr<BoundingBoxLabelItem> label);
	void Remove(const QString& id);
	void RemoveSelected();
	void SetDrawEnble(bool v) { m_IsDrawEnble = v; }
	NRender::BoundingBoxActor* CheckPick(SliceView* view, const glm::dvec3& pos);
	NRender::BoundingBoxActor* CheckContain(SliceView* view, const glm::dvec3& p1, const glm::dvec3& p2);
	void OnColorLabelUpdate();

	void SetBoundingBoxActorEnable(NRender::BoundingBoxActor* actor);
	void SetBoundingBoxActorEnable(const std::string& id);

	virtual void OnFrameChanged(SliceView* view, int value) override;

	virtual void Clear() override;

	virtual void ImportJson(const nlohmann::json& json) override;
	virtual void ExportJson(nlohmann::json& json) override;

private:
	void AddActor(SliceView* view, std::shared_ptr<BoundingBoxLabelItem> label);

private:
	std::map<QString, std::shared_ptr<BoundingBoxLabelItem>> m_LabelMap;
	std::vector<std::weak_ptr<NRender::BoundingBoxActor>> m_LabelActorList;
	bool m_IsDrawEnble = true;
};

