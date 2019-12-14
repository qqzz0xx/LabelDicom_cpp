#include "ColorLabelTableModel.h"
#include "vtkColorTransferFunction.h";
#include "LocalConfig.h"
#include "JsonHelper.h"
#include "GlobalVal.h"
#include "Helper.h"

#include <QStringBuilder>

ColorLabelTableModel::ColorLabelTableModel()
{
	m_Name = typeid(this).name();

	m_VTKLookupTable = vtkSmartPointer<vtkLookupTable>::New();
	m_Model = new QStandardItemModel;
	QStringList strHeader;
	strHeader << "id" << "color" << "desc";
	m_Model->setColumnCount(strHeader.size());
	m_Model->setHorizontalHeaderLabels(strHeader);

	m_ProxyModel = new QSortFilterProxyModel;
	m_ProxyModel->setSourceModel(m_Model);
	m_ProxyModel->setFilterKeyColumn(0);

	m_ParentModel = new QStandardItemModel;
	m_ParentModel->setColumnCount(strHeader.size());
	m_ParentModel->setHorizontalHeaderLabels(strHeader);

	AddColorLabel(QColor(0, 0, 0, 0), "ClearLabel");
	AddColorLabel(QColor(255, 0, 0), "Label 1");
	AddColorLabel(QColor(0, 255, 0), "Label 2");
	AddColorLabel(QColor(0, 0, 255), "Label 3");
	AddColorLabel(QColor(255, 255, 0), "Label 4");
	AddColorLabel(QColor(0, 255, 255), "Label 5");
	AddColorLabel(QColor(255, 0, 255), "Label 6");
}


ColorLabelTableModel::~ColorLabelTableModel()
{
}

std::shared_ptr<ColorLabelItem> ColorLabelTableModel::AddColorLabel(const QColor& color, const QString& desc)
{
	if (m_Model == nullptr) return nullptr;

	auto item = AddColorLabel(m_Index, color, desc);

	return item;
}

std::shared_ptr<ColorLabelItem> ColorLabelTableModel::AddColorLabel(int id, const QColor& color, const QString& desc)
{
	auto item = std::make_shared<ColorLabelItem>();
	item->ID = QString::number(id);
	item->Desc = desc;
	item->Color.setRgba(color.rgba());
	m_ColorTable.emplace(item->ID, item);
	AddItem(item);

	qDebug() << "AddColorLabel" << item->Color;

	if (id + 1 > m_Index)
	{
		m_Index = id + 1;
	}

	return item;
}

//std::shared_ptr<ColorLabelItem> ColorLabelTableModel::AddColorLabel(const std::shared_ptr<ColorLabelItem>& item)
//{
//	if (m_Model == nullptr) return nullptr;
//	m_ColorTable.emplace(item->ID, item);
//	AddItem(item);
//
//	return item;
//}

void ColorLabelTableModel::RemoveColorLabel(const QString& id)
{
	if (id.toInt() == 0) return;

	auto it = m_ColorTable.find(id);
	if (it != m_ColorTable.end())
	{
		m_Model->removeRow(it->second->Item->row());
		m_ColorTable.erase(id);
	}

	if (m_ColorTable.size() == 1)
	{
		m_Index = 1;
	}
}

std::shared_ptr<ColorLabelItem> ColorLabelTableModel::GetColorLabel(const QString& id)
{
	auto it = m_ColorTable.find(id);
	if (it != m_ColorTable.end())
	{
		return it->second;
	}

	return nullptr;
}

void ColorLabelTableModel::UpdateColorLabel(const std::shared_ptr<ColorLabelItem>& item)
{
	auto it = m_ColorTable.find(item->ID);
	if (it != m_ColorTable.end())
	{
		UpdateItem(item);

		qDebug() << "UpdateColorLabel" << item->Color;
	}
}

vtkLookupTable* ColorLabelTableModel::GetLookupTable()
{
	if (m_ColorTable.empty()) return nullptr;
	auto item = m_Model->item(m_Model->rowCount() - 1, 0);
	if (item == nullptr) return nullptr;

	auto endId = item->text().toInt();
	m_VTKLookupTable->SetNumberOfTableValues(endId + 1);
	m_VTKLookupTable->SetTableRange(0, endId);

	for (auto&& [k, v] : m_ColorTable)
	{
		double rgb[4] = { 1,1,1,1 };
		QtHelper::QColorToVTKRGBA(v->Color, rgb);
		auto id = v->ID;
		m_VTKLookupTable->SetTableValue(id.toInt(), rgb);
	}
	m_VTKLookupTable->Build();

	return m_VTKLookupTable;
}

//void ColorLabelTableModel::ImportJson(const QString& path)
//{
//	Clear();
//
//	auto path_str = path.toStdString();
//	LocalConfig conf;
//	conf.SetRootPath(path_str);
//	std::stringstream ss;
//	ss << g_GlobalVal.FileName.toStdString() << "_ColorTable.json";
//	conf.SetFileName(ss.str());
//	conf.Load();
//	auto j = conf.GetJsonData();
//
//}

void ColorLabelTableModel::ImportJson(const nlohmann::json& j)
{
	if (j.is_null()) return;

	Clear();

	AddColorLabel(QColor(0, 0, 0, 0), "ClearLabel");

	for (auto&& var : j)
	{
		QColor color;
		JsonHelper::JsonToQColor(var["Color"], color);
		int id = var["ID"];
		AddColorLabel(id, color, QString::fromStdString(var["Desc"].get<std::string>()));
	}
}

void ColorLabelTableModel::ExportJson(nlohmann::json& j)
{
	auto cmp = [](const auto& p1, const auto& p2)
	{
		return p1.first.toInt() < p2.first.toInt();
	};

	std::set < std::pair<QString, std::shared_ptr<ColorLabelItem>>, decltype(cmp)> s(m_ColorTable.begin(), m_ColorTable.end(), cmp);

	for (auto&& [k, v] : s)
	{
		if (v->ID.toInt() == 0) continue;

		json j_label;
		j_label["ID"] = v->ID.toInt();
		j_label["Desc"] = v->Desc.toStdString();
		JsonHelper::QColorToJson(v->Color, j_label["Color"]);

		j.push_back(j_label);
	}

	//Helper::SaveJson(path, "ColorTable", j);
}

void ColorLabelTableModel::ImportParentJson(const nlohmann::json& j)
{
	if (j.is_null()) return;

	m_GroupModelMap.clear();

	for (auto&& v : j)
	{
		int id = v["ID"];

		auto childs = v["Childs"];
		std::vector<int> list;
		for (auto&& c : childs)
		{
			int cid = c;
			list.emplace_back(cid);
		}

		m_GroupModelMap.emplace(id, list);
	}
	m_GroupModelMap[0] = { 0 };

	UpdateParentModel();
}

void ColorLabelTableModel::ExportParentJson(nlohmann::json& j)
{
	for (auto&& [k, v] : m_GroupModelMap)
	{
		j["ID"] = k;
		nlohmann::json childs_j;
		for (auto&& cid : v)
		{
			childs_j.push_back(cid);
		}
		j["Childs"] = childs_j;
	}
}

void ColorLabelTableModel::Clear()
{
	m_ColorTable.clear();
	m_Model->removeRows(0, m_Model->rowCount());
	m_Index = 0;
}

void ColorLabelTableModel::ForEach(std::function<void(std::shared_ptr<ColorLabelItem>)> func)
{
	for (auto&& [k, v] : m_ColorTable)
	{
		if (v->ID.toInt() != 0)
		{
			func(v);
		}
	}
}

void ColorLabelTableModel::OnParentLabelChanged(QString id)
{
	if (m_GroupModelMap.empty()) return;
	auto it = m_GroupModelMap.find(id.toInt());
	if (it == m_GroupModelMap.end()) return;

	QRegExp reg;
	std::stringstream ss;
	ss << "^(";
	for (int var : it->second)
	{
		ss << var << '|';
	}

	ss << it->first << ")$";

	reg.setPattern(QString::fromStdString(ss.str()));

	m_ProxyModel->setFilterKeyColumn(0);
	m_ProxyModel->setFilterRegExp(reg);
}

void ColorLabelTableModel::AddItem(const std::shared_ptr<ColorLabelItem>& item)
{
	auto icon = QtHelper::CreateColorBoxIcon(16, 16, item->Color);
	int row = m_Model->rowCount();

	auto q = new QStandardItem;
	q->setIcon(icon);
	m_Model->setItem(row, 1, q);

	q = new QStandardItem;
	q->setText(item->ID);
	m_Model->setItem(row, 0, q);
	item->Item = q;

	q = new QStandardItem;
	q->setText(item->Desc);
	m_Model->setItem(row, 2, q);
}

void ColorLabelTableModel::UpdateItem(const std::shared_ptr<ColorLabelItem>& item)
{
	auto row = item->Item->row();
	auto q = m_Model->item(row, 1);
	auto icon = QtHelper::CreateColorBoxIcon(16, 16, item->Color);
	q->setIcon(icon);

	q = m_Model->item(row, 2);
	q->setText(item->Desc);
}

void ColorLabelTableModel::UpdateParentModel()
{
	auto func = [&](auto item, auto model) {
		auto icon = QtHelper::CreateColorBoxIcon(16, 16, item->Color);
		int row = model->rowCount();

		auto q = new QStandardItem;
		q->setIcon(icon);
		model->setItem(row, 1, q);

		q = new QStandardItem;
		q->setText(item->ID);
		model->setItem(row, 0, q);

		q = new QStandardItem;
		q->setText(item->Desc);
		model->setItem(row, 2, q);
	};

	m_ParentModel->removeRows(0, m_Model->rowCount());

	for (auto&& [k, v] : m_GroupModelMap)
	{
		auto label = GetColorLabel(QString::number(k));
		if (label)
		{
			func(label, m_ParentModel);
		}
	}

}

