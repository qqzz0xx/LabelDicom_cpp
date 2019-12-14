#include "LabelDetailModel.h"
#include "MainWindow.h"
#include "AppMain.h"
#include "LabelDetailView.h"
#include "ColorLabelTableModel.h"

#include <QStringList>

LabelDetailModel::LabelDetailModel()
{
	m_Name = typeid(this).name();

	m_Model = new QStandardItemModel;

}

void LabelDetailModel::Add(const std::shared_ptr<ColorLabelItem>& label)
{
	if (!gMainWindow->m_LabelDetailView) return;
	auto id = label->ID.toStdString();
	if (id == "0") return;

	nlohmann::json j;
	j["ID"] = id;
	JsonHelper::QColorToJson(label->Color, j["COLOR"]);
	j["DESC"] = label->Desc.toStdString();

	auto info = gMainWindow->m_LabelDetailView->GetCurDetailInfo();
	for (auto&& [k, v] : info.items())
	{
		j[k] = v;
	}

	auto it = m_LabelDetailTable.find(id);
	if (it == m_LabelDetailTable.end())
	{
		AddItem(j);
	}

	m_LabelDetailTable[label->ID.toStdString()] = j;


}

void LabelDetailModel::Remove(const std::string& id)
{
	if (!gMainWindow->m_LabelDetailView) return;
	m_LabelDetailTable.erase(id);

	auto row = FindItemRow(QString::fromStdString(id));
	if (row != -1)
	{
		m_Model->removeRow(row);
	}

}

void LabelDetailModel::ImportJson(const nlohmann::json& json)
{
	m_LabelDetailOption = nlohmann::json();
	m_LabelDetailTable = nlohmann::json();
	m_HeaderStr.clear();
	m_Model->clear();

	JsonHelper::TryGetValue(json, "option", m_LabelDetailOption);
	JsonHelper::TryGetValue(json, "table", m_LabelDetailTable);

	if (!m_LabelDetailOption.is_null())
	{
		m_HeaderStr << "ID" << "COLOR" << "DESC";

		for (auto&& v : m_LabelDetailOption)
		{
			m_HeaderStr << QString::fromStdString(v["title"]);
		}

		m_Model->setColumnCount(m_HeaderStr.size());
		m_Model->setHorizontalHeaderLabels(m_HeaderStr);

		if (gMainWindow->m_LabelDetailView)
		{
			gMainWindow->m_LabelDetailView->LoadOption(m_LabelDetailOption);
		}
	}

	if (!m_LabelDetailTable.is_null())
	{
		for (auto&& [k, v] : m_LabelDetailTable.items())
		{
			AddItem(v);
		}
	}
}

void LabelDetailModel::ExportJson(nlohmann::json& json)
{
	if (m_LabelDetailTable.is_null() || m_LabelDetailOption.is_null()) return;

	json["option"] = m_LabelDetailOption;
	json["table"] = m_LabelDetailTable;
}

nlohmann::json LabelDetailModel::GetLabelDetailOptionMap()
{
	nlohmann::json j;
	for (auto&& var : m_LabelDetailOption)
	{
		std::string title = var["title"];
		j.emplace(title, var);
	}
	return j;
}

void LabelDetailModel::Apply(QString colorId, nlohmann::json& json)
{
	auto row = FindItemRow(colorId);
	if (row == -1) return;
	for (size_t i = 3; i < m_HeaderStr.size(); i++)
	{
		auto title = m_HeaderStr[i].toStdString();
		std::string value = json[title];

		auto item = m_Model->item(row, i);
		if (item)
		{
			item->setText(QString::fromStdString(value));
		}

		m_LabelDetailTable[colorId.toStdString()][title] = value;

	}
}

void LabelDetailModel::AddItem(const nlohmann::json& json)
{
	int row = m_Model->rowCount();
	int i = 0;

	for (auto k : m_HeaderStr)
	{
		auto v = json[k.toStdString()];

		if (k == "COLOR")
		{
			QColor color;
			JsonHelper::JsonToQColor(v, color);
			auto icon = QtHelper::CreateColorBoxIcon(16, 16, color);

			auto item = new QStandardItem();
			item->setIcon(icon);
			m_Model->setItem(row, i, item);
		}
		else
		{
			auto item = new QStandardItem(QString::fromStdString(v));
			m_Model->setItem(row, i, item);
		}
		i++;
	}

}

int LabelDetailModel::FindItemRow(const QString& id)
{
	int row = m_Model->rowCount();
	for (int i = 0; i < row; i++)
	{
		auto item = m_Model->item(i, 0);
		if (item && item->text() == id)
		{
			return i;
		}
	}

	return -1;
}
