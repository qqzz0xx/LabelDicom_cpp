#include "FrameLabelModel.h"
#include "SliceView.h"
#include "LocalConfig.h"
#include "GlobalVal.h"
#include "ColorLabelTableModel.h"
#include "AppMain.h"
#include "JsonHelper.h"

using namespace NRender;

FrameLabelModel::FrameLabelModel()
{
	m_Name = typeid(this).name();
}

void FrameLabelModel::Add(std::shared_ptr<FrameLabelItem> p)
{
	if (p == nullptr || p->View == nullptr) return;

	//int idx = (int)p->ItemType + 100 * (int)p->ViewType;

	//auto GetTextActorFunc = [&](int  idx) {
	//	auto it = m_TextActorMap.find(idx);
	//	if (it == m_TextActorMap.end() || it->second.lock() == nullptr)
	//	{
	//		auto actor = std::make_shared<TextActor2D>();
	//		p->View->GetRenderEngine()->GetSceneManager()->AddNodeToGlobleScene(actor);
	//		m_TextActorMap[idx] = actor;
	//	}

	//	if (auto actor = m_TextActorMap[idx].lock(); actor)
	//	{
	//		actor->SetText(p->LabelType->Desc.toStdString());

	//		return actor;
	//	}
	//};

	double rgb[4];
	QtHelper::QColorToVTKRGBA(p->LabelType->Color, rgb);

	if (p->ItemType == FrameLabelType_Frame)
	{
		if (p->LabelType->ID.toInt() == 0)
		{
			Remove(p->View, p->FrameCount);
			return;
		}

		bool find = false;
		for (auto& var : m_FrameLabels)
		{
			if (var->ViewType == p->ViewType && var->FrameCount == p->FrameCount)
			{
				var->LabelType = p->LabelType;
				find = true;
				break;
			}
		}
		if (!find)
		{
			m_FrameLabels.emplace_back(p);
		}


		auto bytes = p->LabelType->Desc.toUtf8();

		p->View->SetLabelDesc(bytes.toStdString(), 0);
		p->View->SetLabelDescColor(rgb, 0);
	}
	else if (p->ItemType == FrameLabelType_Illness)
	{
		//auto actor = GetTextActorFunc(idx);
		//if (actor)
		//{
		//	actor->SetPosition(glm::dvec2(0, 400));
		//	double rgb[3] = { 1,0.4,0.7 };
		//	actor->SetColor(rgb);

		//	auto txt = p->LabelType->ID.toInt() == 0 ? "" : p->LabelType->Desc.toStdString();
		//	actor->SetText(txt);
		//}
		auto txt = p->LabelType->ID.toInt() == 0 ? "" : p->LabelType->Desc.toUtf8().toStdString();
		p->View->SetLabelDesc(txt, 1);
		p->View->SetLabelDescColor(rgb, 1);
		m_FrameTexts[1] = txt;
		m_IllnessLabelItem = p;
	}
	else if (p->ItemType == FrameLabelType_Struct)
	{
		auto txt = p->LabelType->ID.toInt() == 0 ? "" : p->LabelType->Desc.toUtf8().toStdString();
		p->View->SetLabelDesc(txt, 2);
		p->View->SetLabelDescColor(rgb, 2);
		m_FrameTexts[2] = txt;
		m_StructLabelItem = p;
	}
}

void FrameLabelModel::Remove(SliceView* view, int FrameCount)
{
	view->SetLabelDesc("", 0);

	for (auto it = m_FrameLabels.begin(); it != m_FrameLabels.end(); it++)
	{
		if ((*it)->FrameCount == FrameCount && (*it)->ViewType == view->GetMprType())
		{
			m_FrameLabels.erase(it);
			break;
		}
	}

	auto fc = gMainWindow->GetFrameComponent();
	if (fc)
	{
		fc->RefreshIconListColor();
	}
}

void FrameLabelModel::Clear()
{
	for (auto&& var : m_FrameLabels)
	{
		var->View->SetLabelDesc("", 0);
	}
	m_FrameLabels.clear();
	m_IllnessLabelItem = nullptr;
	m_StructLabelItem = nullptr;
}

void FrameLabelModel::OnFrameChanged(SliceView* view, int value)
{
	view->SetLabelDesc("", 0);
	auto type = view->GetMprType();

	for (auto&& var : m_FrameLabels)
	{
		if (var->ViewType == type && var->FrameCount == value)
		{
			view->SetLabelDesc(var->LabelType->Desc.toStdString(), 0);
		}
	}
}

void FrameLabelModel::ExportJson(nlohmann::json& j_labels)
{
	if (m_FrameLabels.empty() && !m_IllnessLabelItem && !m_StructLabelItem) return;

	auto func = [&](const auto& var)
	{
		if (var == nullptr)
		{
			return nlohmann::json();
		}
		json j;
		j["ViewType"] = (int)var->ViewType;
		j["FrameCount"] = var->FrameCount;
		j["Label"] = var->LabelType->ID.toInt();
		j["ItemType"] = (int)var->ItemType;
		return j;
	};

	nlohmann::json j_frame;
	for (auto&& var : m_FrameLabels)
	{
		json j = func(var);

		j_frame.push_back(j);
	}

	j_labels["FrameLabel"] = j_frame;
	j_labels["StructLabel"] = func(m_StructLabelItem);
	j_labels["IllnessLabel"] = func(m_IllnessLabelItem);

	//LocalConfig local;
	//local.SetRootPath(path.toStdString());
	//std::stringstream ss;
	//ss << g_GlobalVal.FileName.toStdString() << "_FrameLabel.json";
	//local.SetFileName(ss.str());
	//local.SetValue("FileName", g_GlobalVal.FileName.toStdString());
	//local.SetJsonData(j_labels);
	//local.Save();
}

void FrameLabelModel::ImportJson(const nlohmann::json& json)
{
	auto curSliceVals = gMainWindow->GetCurSliceVal();
	nlohmann::json j_frame;
	JsonHelper::TryGetValue(json, "FrameLabel", j_frame);

	auto func = [&](const auto& j)
	{ 
		auto var = std::make_shared<FrameLabelItem>();
		if (j.is_null()) return var;

		var->ViewType = j["ViewType"];
		var->FrameCount = j["FrameCount"];
		var->ItemType = j["ItemType"];
		int colorID = j["Label"];
		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		var->LabelType = model->GetColorLabel(QString::number(colorID));

		auto sv = gMainWindow->GetSliceView(var->ViewType);
		var->View = sv;

		return var;
	};

	for (auto&& j : j_frame)
	{
		auto var = func(j);

		if (var->View && var->FrameCount == curSliceVals[var->ViewType])
		{
			var->View->SetLabelDesc(var->LabelType->Desc.toStdString(), 0);
		}

		m_FrameLabels.emplace_back(var);
	}


	auto AddLabelFunc = [&](auto&& name)
	{
		nlohmann::json j;
		JsonHelper::TryGetValue(json, name, j);
		auto var = func(j);
		Add(var);
	};

	AddLabelFunc("StructLabel");
	AddLabelFunc("IllnessLabel");

	auto fc = gMainWindow->GetFrameComponent();
	if (fc)
	{
		fc->RefreshIconListColor();
	}
}

void FrameLabelModel::ForEach(std::function<void(std::shared_ptr<FrameLabelItem>)> func)
{
	for (auto&& var : m_FrameLabels)
	{
		func(var);
	}
}
