#pragma once
#include "Header.h"
#include "ModelBase.h"
#include "QtHelper.h"
#include <QStandardItemModel>
#include <QStandardItem>
#include <QString>
#include <QSortFilterProxyModel>

class vtkColorTransferFunction;

struct ColorLabelItem
{
	QString ID;
	QColor Color;
	QString Desc;
	QStandardItem* Item;

};
class ColorLabelTableModel : public ModelBase
{
public:
	ColorLabelTableModel();
	~ColorLabelTableModel();

	QStandardItemModel* GetModel() { return m_Model; }
	QSortFilterProxyModel* GetProxyModel() { return m_ProxyModel; }
	QStandardItemModel* GetParentModel() { return m_ParentModel; }

	std::shared_ptr<ColorLabelItem> AddColorLabel(const QColor& color, const QString& desc);
	//std::shared_ptr<ColorLabelItem> AddColorLabel(const std::shared_ptr<ColorLabelItem>& item);
	void RemoveColorLabel(const QString& id);
	std::shared_ptr<ColorLabelItem> GetColorLabel(const QString& id);
	void UpdateColorLabel(const std::shared_ptr<ColorLabelItem>& item);
	vtkLookupTable* GetLookupTable();

	virtual void ImportJson(const nlohmann::json& j) override;
	virtual void ExportJson(nlohmann::json& j) override;

	void ImportParentJson(const nlohmann::json& j);
	void ExportParentJson(nlohmann::json& j);

	virtual void Clear() override;
	void ForEach(std::function<void(std::shared_ptr<ColorLabelItem>)> func);

	void OnParentLabelChanged(QString id);

private:
	std::shared_ptr<ColorLabelItem> AddColorLabel(int id, const QColor& color, const QString& desc);
	void AddItem(const std::shared_ptr<ColorLabelItem>& item);
	void UpdateItem(const std::shared_ptr<ColorLabelItem>& item);
	void UpdateParentModel();

	QStandardItemModel* m_Model;
	QSortFilterProxyModel* m_ProxyModel;
	QStandardItemModel* m_ParentModel;

	int m_Index = 0;
	std::map<QString, std::shared_ptr<ColorLabelItem>> m_ColorTable;
	vtkSmartPointer<vtkLookupTable> m_VTKLookupTable;
	vtkSmartPointer<vtkColorTransferFunction> m_ColorFunc;

	std::map<int, std::vector<int>> m_GroupModelMap;
};

