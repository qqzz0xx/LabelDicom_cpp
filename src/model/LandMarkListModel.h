#pragma once
#include "Types.h"
#include "Header.h"
#include "Structs.h"
#include "ModelBase.h"
struct ColorLabelItem;

namespace NRender
{
	class LandMarkActor;
	class LineActor;
}
class SliceView;

struct LandMarkListItem : public UObject
{
	QString ImagePath;
	MprType ViewType;
	int ImageIndex;
	glm::dvec3 Pos;
	std::shared_ptr<ColorLabelItem> LabelType;
	QStandardItem* Item;
};

struct ConnectLine
{
	MprType SliceType;
	std::vector<int> Ids;
	std::vector<std::weak_ptr<NRender::LineActor>> m_LineActors;
};

class LandMarkListModel : public ModelBase
{
public:
	LandMarkListModel();
	~LandMarkListModel();
	QStandardItemModel* GetModel() { return m_Model; }
	std::weak_ptr<NRender::LandMarkActor> Add(SliceView* view, std::shared_ptr<LandMarkListItem> label);
	void Remove(const QString& id);
	LandMarkListItem* Find(const QString& id);

	void OnFrameChanged(SliceView* view, int v);
	NRender::LandMarkActor* CheckPickActor(double* pos);
	void DoPaste(SliceView* view);

	void SaveToJson(const QString& path);

	virtual void Clear() override;
	virtual void Start() override;
	virtual void Exit() override;

	virtual void ExportJson(nlohmann::json& j) override;
	virtual void ImportJson(const nlohmann::json& json) override;

	void UpdateLines(SliceView* view);

	const std::map<QString, std::shared_ptr<LandMarkListItem>>& GetData() { return m_LabelMap; }

private:
	std::weak_ptr<NRender::LandMarkActor> AddActor(SliceView* view, std::shared_ptr<LandMarkListItem> label);
	void AddItem(std::shared_ptr<LandMarkListItem> label);
	void RemoveItem(std::shared_ptr<LandMarkListItem> label);
	void RemoveActor(std::shared_ptr<LandMarkListItem> label);
	void ImportLineJson(nlohmann::json& json);

	QStandardItemModel* m_Model;
	std::map<QString, std::shared_ptr<LandMarkListItem>> m_LabelMap;
	std::vector<std::weak_ptr<NRender::LandMarkActor>> m_LabelActorList;
	int m_SliceVal = 0;
	bool m_IsSliceMoveUp = false;

	std::vector<ConnectLine> m_Lines;
	nlohmann::json m_LinesData;

};

