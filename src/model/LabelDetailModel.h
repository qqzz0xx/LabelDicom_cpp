#pragma once
#include "ModelBase.h"

struct ColorLabelItem;

class LabelDetailModel : public ModelBase
{
public:
	LabelDetailModel();

	void Add(const std::shared_ptr<ColorLabelItem>& label);
	void Remove(const std::string& id);

	virtual void ImportJson(const nlohmann::json& json) override;
	virtual void ExportJson(nlohmann::json& json) override;
	QStringList GetTableHeader() { return m_HeaderStr; }

	nlohmann::json GetLabelDetailOption() { return m_LabelDetailOption; }
	nlohmann::json GetLabelDetailOptionMap();
	nlohmann::json GetLabelDetailTable() { return m_LabelDetailTable; }
	void Apply(QString colorId, nlohmann::json& json);


	QStandardItemModel* GetModel() { return m_Model; }
private:
	void AddItem(const nlohmann::json& json);

	int FindItemRow(const QString& id);

private:
	nlohmann::json m_LabelDetailOption;
	nlohmann::json m_LabelDetailTable;
	QStringList m_HeaderStr;
	QStandardItemModel* m_Model;
};

