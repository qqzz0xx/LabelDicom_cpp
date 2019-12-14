#include "LandMarkListModel.h"
#include "LandMarkActor.h"
#include "ColorLabelTableModel.h"
#include "QtHelper.h"
#include "SliceView.h"
#include "LocalConfig.h"
#include "JsonHelper.h"
#include "GlobalVal.h"
#include "AppMain.h"
#include "MainWindow.h"
#include "LineActor.h"
#include "json.hpp"
#include "CoordHelper.h"

using namespace NRender;

LandMarkListModel::LandMarkListModel()
{
	m_Name = typeid(this).name();

	m_Model = new QStandardItemModel;
	QStringList strHeader;
	strHeader << "ID" << "COLOR" << "DESC" << "FRAME";
	m_Model->setColumnCount(strHeader.size());
	m_Model->setHorizontalHeaderLabels(strHeader);
}


LandMarkListModel::~LandMarkListModel()
{
}

std::weak_ptr<NRender::LandMarkActor> LandMarkListModel::Add(SliceView* view, std::shared_ptr<LandMarkListItem> label)
{
	if (!g_ConfigVal.CanRepeatLabel)
	{
		for (auto&& actor : m_LabelActorList)
		{
			if (auto p = actor.lock(); p && p->GetActive()
				&& p->GetRenderEngine() == view->GetRenderEngine())
			{
				auto id = p->GetUUID();
				auto item = Find(QString::fromStdString(id));
				if (item->LabelType->ID == label->LabelType->ID)
				{
					return std::weak_ptr<NRender::LandMarkActor>();
				}
			}
		}
	}

	m_LabelMap.emplace(QString::fromStdString(label->GetUUID()), label);
	AddItem(label);
	auto actor = AddActor(view, label);

	auto fc = gMainWindow->GetFrameComponent();
	if (fc) 
	{
		fc->RefreshIconListColor();
	}

	return actor;
}

void LandMarkListModel::Remove(const QString& id)
{
	auto it = m_LabelMap.find(id);
	if (it != m_LabelMap.end())
	{
		RemoveItem(it->second);
		RemoveActor(it->second);
		auto view = gMainWindow->GetSliceView(it->second->ViewType);

		UpdateLines(view);

		m_LabelMap.erase(id);

		gMainWindow->RenderAll();
	}

	auto fc = gMainWindow->GetFrameComponent();
	if (fc)
	{
		fc->RefreshIconListColor();
	}
}

LandMarkListItem* LandMarkListModel::Find(const QString& id)
{
	auto it = m_LabelMap.find(id);
	if (it != m_LabelMap.end())
	{
		return it->second.get();
	}

	return nullptr;
}

void LandMarkListModel::OnFrameChanged(SliceView* view, int i)
{
	m_IsSliceMoveUp = m_SliceVal > i;
	m_SliceVal = i;

	for (auto& p : m_LabelActorList)
	{
		if (auto var = p.lock(); var != nullptr
			&& var->GetRenderEngine() == view->GetRenderEngine())
		{
			var->SetActive(false);
		}
	}


	for (auto&& [k, v] : m_LabelMap)
	{
		if (i == v->ImageIndex && view->GetMprType() == v->ViewType)
		{
			AddActor(view, v);
		}
	}

	UpdateLines(view);
}

NRender::LandMarkActor* LandMarkListModel::CheckPickActor(double* pos)
{
	auto t_pos = glm::make_vec2(pos);
	for (auto& p : m_LabelActorList)
	{
		if (auto var = p.lock(); var && var->GetActive() == true)
		{
			auto p = glm::make_vec2(var->GetActor()->GetPosition());
			double d = glm::length(t_pos - p);
			if (d < 10)
			{
				return var.get();
			}
		}
	}

	return nullptr;
}

void LandMarkListModel::DoPaste(SliceView* view)
{
	std::unordered_map<int, std::vector<std::shared_ptr<LandMarkListItem>>> map;

	for (auto&& [k, v] : m_LabelMap)
	{
		auto& list = map[v->ImageIndex];
		list.emplace_back(v);
	}

	int step = m_IsSliceMoveUp ? 1 : -1;
	bool isFinded = false;
	for (size_t i = 1; i < 100; i++)
	{
		auto it = map.find(m_SliceVal + step * i);

		if (it != map.end())
		{
			for (auto&& var : it->second)
			{
				auto it = std::make_shared<LandMarkListItem>();
				it->ImageIndex = m_SliceVal;
				it->LabelType = var->LabelType;
				it->Pos = var->Pos;
				it->ViewType = var->ViewType;
				Add(view, it);
			}
			isFinded = true;
			break;
		}
	}

	if (!isFinded)
	{
		step = -step;
		for (size_t i = 1; i < 100; i++)
		{
			auto it = map.find(m_SliceVal + step * i);

			if (it != map.end())
			{
				for (auto&& var : it->second)
				{
					auto it = std::make_shared<LandMarkListItem>();
					it->ImageIndex = m_SliceVal;
					it->LabelType = var->LabelType;
					it->Pos = var->Pos;
					it->ViewType = var->ViewType;
					Add(view, it);
				}
				break;
			}
		}
	}

	UpdateLines(view);

	gMainWindow->RenderAll();
}

void LandMarkListModel::SaveToJson(const QString& path)
{
	if (m_LabelMap.empty()) return;

	auto path_str = path.toStdString();
	std::unordered_map<int, std::vector<std::shared_ptr<LandMarkListItem>>> map;

	for (auto&& [k, v] : m_LabelMap)
	{
		auto& list = map[v->ImageIndex];
		list.emplace_back(v);
	}

	json j_labels;

	for (auto&& [k, v] : map)
	{
		json j_labelList;
		for (auto&& label : v)
		{
			json j;
			json j_pos;
			JsonHelper::Vec3ToJson(label->Pos, j_pos);
			j["Position"] = j_pos;
			j["Label"] = label->LabelType->ID.toInt();
			j_labelList.push_back(j);
		}

		json j_slice_labels;
		j_slice_labels["ImageIndex"] = k;
		j_slice_labels["LabelList"] = j_labelList;
		j_labels.push_back(j_slice_labels);
	}

	LocalConfig local;
	local.SetRootPath(path_str);
	std::stringstream ss;
	ss << g_GlobalVal.FileName.toStdString() << "_LandMark.json";
	local.SetFileName(ss.str());
	local.SetValue("FileName", g_GlobalVal.FileName.toStdString());
	local.SetValue("LandMark:", j_labels);
	local.Save();
}

void LandMarkListModel::ExportJson(nlohmann::json& j)
{
	if (m_LabelMap.empty()) return;

	nlohmann::json j_labels;

	std::unordered_map<int, std::vector<std::shared_ptr<LandMarkListItem>>> map;

	for (auto&& [k, v] : m_LabelMap)
	{
		int mask = v->ImageIndex;
		uint viewType = v->ViewType;
		uint idx = (mask << 4) | viewType;
		auto& list = map[idx];
		list.emplace_back(v);
	}

	for (auto&& [k, v] : map)
	{
		json j_labelList;
		for (auto&& label : v)
		{
			json j;
			json j_pos;
			JsonHelper::Vec3ToJson(label->Pos, j_pos);
			j["Position"] = j_pos;
			j["Label"] = label->LabelType->ID.toInt();
			j_labelList.push_back(j);
		}

		json j_slice_labels;

		 
		j_slice_labels["ImageIndex"] = ((uint)k) >> 4;
		j_slice_labels["ViewType"] = ((uint)k) & 0xF;
		j_slice_labels["LabelList"] = j_labelList;
		j_labels.push_back(j_slice_labels);
	}

	nlohmann::json j_lines;
	for (auto&& line : m_Lines)
	{
		if (line.SliceType == MprType::Axial || line.SliceType == MprType::None)
		{
			nlohmann::json j_line;
			j_line["Ids"] = nlohmann::json();
			for (auto&& id : line.Ids)
			{
				j_line["Ids"].push_back(id);
			}

			j_lines.push_back(j_line);
		}
	}


	j["Points"] = j_labels;
	j["Lines"] = j_lines;

}

void LandMarkListModel::Clear()
{
	m_Model->removeRows(0, m_Model->rowCount());
	m_LabelActorList.clear();
	m_LabelMap.clear();
	m_Lines.clear();
}

void LandMarkListModel::Start()
{
	ImportLineJson(m_LinesData);
}

void LandMarkListModel::Exit()
{
	Clear();
}

void LandMarkListModel::ImportJson(const nlohmann::json& json)
{
	m_LinesData = json["Lines"];
	ImportLineJson(m_LinesData);

	auto j_points = json["Points"];

	auto curSliceVals = gMainWindow->GetCurSliceVal();

	for (auto&& j_slice : j_points)
	{
		int sliceVal = j_slice["ImageIndex"];
		int viewType = j_slice["ViewType"];
		for (auto&& j_point : j_slice["LabelList"])
		{
			auto it = std::make_shared<LandMarkListItem>();
			it->ImageIndex = sliceVal;
			int colorID = j_point["Label"];
			auto model = gModelMgr->GetModel<ColorLabelTableModel>();
			it->LabelType = model->GetColorLabel(QString::number(colorID));
			JsonHelper::JsonToVec3(j_point["Position"], it->Pos);
			it->ViewType = (MprType)viewType;
			auto sv = gMainWindow->GetSliceView(it->ViewType);
			
			m_LabelMap.emplace(QString::fromStdString(it->GetUUID()), it);
			AddItem(it);

			if (sv && it->ImageIndex == curSliceVals[viewType])
			{
				AddActor(sv, it);
			}
		}
	}

	
	UpdateLines(gMainWindow->GetSliceView(MprType::Axial));
	UpdateLines(gMainWindow->GetSliceView(MprType::Sagittal));
	UpdateLines(gMainWindow->GetSliceView(MprType::Coronal));
	UpdateLines(gMainWindow->GetSliceView(MprType::None));

	auto fc = gMainWindow->GetFrameComponent();
	if (fc)
	{
		fc->RefreshIconListColor();
	}
}

void LandMarkListModel::UpdateLines(SliceView* view)
{
	if (view == nullptr) return;

	auto findIdFunc = [&](int id)
	{
		for (auto&& actor : m_LabelActorList)
		{
			if (auto p = actor.lock(); p && p->GetActive())
			{
				auto uuid = p->GetUUID();
				auto label = Find(QString::fromStdString(uuid));
				if (label != nullptr)
				{
					if (label->LabelType->ID.toInt() == id)
					{
						return actor;
					}
				}
			}
		}

		return std::weak_ptr<NRender::LandMarkActor>();
	};

	for (auto&& var : m_Lines)
	{
		if (var.SliceType == view->GetMprType())
		{
			for (auto&& actor : var.m_LineActors)
			{
				if (auto p = actor.lock(); p)
				{
					p->SetActive(false);
				}
			}
		}
	}

	for (auto&& line : m_Lines)
	{
		bool isFindedAll = true;
		std::vector<std::shared_ptr<NRender::LandMarkActor>> points;

		for (auto&& id : line.Ids)
		{
			auto actor = findIdFunc(id);
			auto p = actor.lock();
			if (p == nullptr)
			{
				isFindedAll = false;
				break;
			}
			points.push_back(p);
		}

		if (isFindedAll)
		{
			for (size_t i = 0; i < points.size() - 1; i++)
			{
				auto p1 = points[i]->GetActor()->GetPosition();
				auto p2 = points[i + 1]->GetActor()->GetPosition();

				if (auto p = line.m_LineActors[i].lock(); p)
				{
					p->SetPos1(p1);
					p->SetPos2(p2);
					p->SetActive(true);
				}
			}
		}
	}
}

std::weak_ptr<NRender::LandMarkActor> LandMarkListModel::AddActor(SliceView* view, std::shared_ptr<LandMarkListItem> label)
{
	auto func = [&](auto var) {
		double pos[3] = { label->Pos.x, label->Pos.y, 0 };

		CoordHelper::ImageXYToWorld(pos, view->GetSingleImageActor(), pos);

		var->SetPosition(pos);
		double rgb[3] = { 1,0,0 };
		QtHelper::QColorToVTKRGB(label->LabelType->Color, rgb);
		var->SetLabelColor(rgb);
		var->SetLabelDesc(label->LabelType->Desc);
		var->SetUUID(label->GetUUID());
		var->SetActive(true);
	};


	std::weak_ptr<NRender::LandMarkActor> retActor;
	bool isFinded = false;
	for (auto& p : m_LabelActorList)
	{
		if (auto var = p.lock(); var->GetActive() == false
			&& var->GetRenderEngine() == view->GetRenderEngine())
		{
			retActor = var;
			func(var);
			isFinded = true;
			break;
		}
	}

	if (isFinded == false)
	{
		auto actor = std::make_shared<LandMarkActor>();
		auto smgr = view->GetRenderEngine()->GetSceneManager();
		smgr->AddNodeToGlobleScene(actor);
		func(actor);
		m_LabelActorList.emplace_back(actor);

		retActor = actor;
	}



	return retActor;
}

void LandMarkListModel::AddItem(std::shared_ptr<LandMarkListItem> label)
{
	int row = m_Model->rowCount();

	auto item = label->LabelType;

	auto q = new QStandardItem;
	auto icon = QtHelper::CreateColorBoxIcon(16, 16, item->Color);
	q->setIcon(icon);
	m_Model->setItem(row, 1, q);

	q = new QStandardItem;
	q->setText(item->ID);
	m_Model->setItem(row, 0, q);

	q = new QStandardItem;
	q->setText(item->Desc);
	m_Model->setItem(row, 2, q);

	auto quuid = new QStandardItemUUID;
	quuid->setText(QString::number(label->ImageIndex));
	quuid->uuid = label->GetUUID();

	m_Model->setItem(row, 3, quuid);

	label->Item = q;
}

void LandMarkListModel::RemoveItem(std::shared_ptr<LandMarkListItem> label)
{
	auto idx = m_Model->indexFromItem(label->Item);
	m_Model->removeRow(idx.row());
}

void LandMarkListModel::RemoveActor(std::shared_ptr<LandMarkListItem> label)
{
	for (auto& p : m_LabelActorList)
	{
		if (auto var = p.lock(); var->GetUUID() == label->GetUUID())
		{
			var->SetActive(false);
		}
	}
}

void LandMarkListModel::ImportLineJson(nlohmann::json& j_lines)
{
	if (j_lines.is_null()) return;

	auto initActorFunc = [&](ConnectLine& line)
	{
		for (size_t i = 0; i < line.Ids.size() - 1; i++)
		{
			auto lineActor = std::make_shared<LineActor>();
			lineActor->SetAlpha(0.6);
			line.m_LineActors.emplace_back(lineActor);

			auto sv = gMainWindow->GetSliceView(line.SliceType);
			if (sv)
			{
				sv->GetRenderEngine()->GetSceneManager()->AddNodeToGlobleScene(lineActor);
				lineActor->SetEnableDist(true);
			}
		}
	};

	for (auto&& line : j_lines)
	{
		auto sliceType = gMainWindow->GetSliceType();
		if (sliceType == SliceType_VoxSlice)
		{
			for (size_t i = 0; i < 3; i++)
			{
				ConnectLine stLine;
				for (int&& id : line["Ids"])
				{
					stLine.Ids.push_back(id);
				}
				stLine.SliceType = (MprType)i;
				initActorFunc(stLine);
				m_Lines.emplace_back(stLine);
			}

		}
		else
		{
			ConnectLine stLine;
			stLine.SliceType = MprType::None;

			for (int&& id : line["Ids"])
			{
				stLine.Ids.push_back(id);
			}

			initActorFunc(stLine);
			m_Lines.emplace_back(stLine);
		}
	}
}
