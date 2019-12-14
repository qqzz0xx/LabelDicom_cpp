#include "BoundingBoxLabelModel.h"
#include "BoundingBoxActor.h"
#include "SliceView.h"
#include "AppMain.h"
#include "vtkBoundingBox.h"
#include "JsonHelper.h"

BoundingBoxLabelModel::BoundingBoxLabelModel()
{
	m_Name = typeid(BoundingBoxLabelModel*).name();
}

void BoundingBoxLabelModel::Add(SliceView* view, std::shared_ptr<BoundingBoxLabelItem> label)
{
	if (!m_IsDrawEnble || label->LabelType->ID.toInt() == 0) return;
	auto d = label->Bounding[0] - label->Bounding[1];
	if (glm::abs(d.x) < 5 || glm::abs(d.y) < 5) return;

	m_LabelMap.emplace(QString::fromStdString(label->GetUUID()), label);

	AddActor(view, label);

	SetBoundingBoxActorEnable(label->GetUUID());
}

void BoundingBoxLabelModel::Remove(const QString& id)
{
	auto it = m_LabelMap.find(id);
	auto s_id = id.toStdString();
	if (it != m_LabelMap.end())
	{
		m_LabelMap.erase(id);

		for (auto&& var : m_LabelActorList)
		{
			if (auto p = var.lock(); p && p->GetUUID() == s_id)
			{
				p->SetActive(false);
			}
		}

		gMainWindow->RenderAll();
	}
}

void BoundingBoxLabelModel::RemoveSelected()
{
	for (auto&& var : m_LabelActorList)
	{
		if (auto p = var.lock(); p && p->GetActive())
		{
			if (p->GetEnable())
			{
				Remove(QString::fromStdString(p->GetUUID()));
				return;
			}
		}
	}
}

NRender::BoundingBoxActor* BoundingBoxLabelModel::CheckPick(SliceView* view, const glm::dvec3& pos)
{
	for (auto&& var : m_LabelActorList)
	{
		if (auto p = var.lock(); p && p->GetActive()
			&& p->GetRenderEngine() == view->GetRenderEngine())
		{
			if (p->CheckHitBox(pos))
			{
				return p.get();
			}
		}
	}

	return nullptr;
}

NRender::BoundingBoxActor* BoundingBoxLabelModel::CheckContain(SliceView* view, const glm::dvec3& p1, const glm::dvec3& p2)
{
	vtkBoundingBox box;
	box.AddPoint((double*)glm::value_ptr(p1));
	box.AddPoint((double*)glm::value_ptr(p2));

	for (auto&& var : m_LabelActorList)
	{
		if (auto p = var.lock(); p && p->GetActive()
			&& p->GetRenderEngine() == view->GetRenderEngine())
		{
			vtkBoundingBox pbox;
			pbox.AddPoint((double*)glm::value_ptr(p->m_Points[0]));
			pbox.AddPoint((double*)glm::value_ptr(p->m_Points[1]));
			if (box.Contains(pbox))
			{
				return p.get();
			}
		}
	}

	return nullptr;
}

void BoundingBoxLabelModel::OnColorLabelUpdate()
{
	for (auto&& var : m_LabelActorList)
	{
		if (auto p = var.lock(); p && p->GetActive())
		{
			p->UpdateColor();
		}
	}
}

void BoundingBoxLabelModel::SetBoundingBoxActorEnable(NRender::BoundingBoxActor* actor)
{
	for (auto&& var : m_LabelActorList)
	{
		if (auto p = var.lock(); p && p->GetActive())
		{
			p->SetEnable(false);
		}
	}

	if (actor)
	{
		actor->SetEnable(true);
	}

	//gMainWindow->RenderAll();
}

void BoundingBoxLabelModel::SetBoundingBoxActorEnable(const std::string& id)
{
	for (auto&& var : m_LabelActorList)
	{
		if (auto p = var.lock(); p && p->GetActive())
		{
			if (p->GetUUID() == id)
			{
				p->SetEnable(true);
			}
			else
			{
				p->SetEnable(false);
			}
		}
	}
}

void BoundingBoxLabelModel::OnFrameChanged(SliceView* view, int value)
{
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
		if (value == v->FrameCount && view->GetMprType() == v->SliceType)
		{
			AddActor(view, v);
		}
	}
}

void BoundingBoxLabelModel::Clear()
{
	for (auto& p : m_LabelActorList)
	{
		if (auto var = p.lock(); var != nullptr)
		{
			var->SetActive(false);
		}
	}

	m_LabelActorList.clear();
	m_LabelMap.clear();
}

void BoundingBoxLabelModel::ImportJson(const nlohmann::json& json)
{

	auto curSliceVals = gMainWindow->GetCurSliceVal();

	for (auto&&j : json)
	{
		auto p = std::make_shared<BoundingBoxLabelItem>();
		p->SliceType = j["SliceType"];
		JsonHelper::JsonToVec3(j["p1"], p->Bounding[0]);
		JsonHelper::JsonToVec3(j["p2"], p->Bounding[1]);
		p->FrameCount = j["FrameCount"];
		int colorID = j["Label"];
		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		p->LabelType = model->GetColorLabel(QString::number(colorID));

		m_LabelMap.emplace(QString::fromStdString(p->GetUUID()), p);

		auto sv = gMainWindow->GetSliceView(p->SliceType);
		if (sv && curSliceVals[p->SliceType] == p->FrameCount)
		{
			AddActor(sv, p);
		}
	}
}

void BoundingBoxLabelModel::ExportJson(nlohmann::json& json)
{
	for (auto&& [k, v] : m_LabelMap)
	{
		nlohmann::json j;
		j["SliceType"] = v->SliceType;
		JsonHelper::Vec3ToJson(v->Bounding[0], j["p1"]);
		JsonHelper::Vec3ToJson(v->Bounding[1], j["p2"]);
		j["FrameCount"] = v->FrameCount;
		j["Label"] = v->LabelType->ID.toInt();

		json.emplace_back(j);
	}
}

void BoundingBoxLabelModel::AddActor(SliceView* view, std::shared_ptr<BoundingBoxLabelItem> label)
{
	auto func = [&](auto p)
	{
		p->Init(label);
		p->SetUUID(label->GetUUID());
		p->SetPoints(label->Bounding[0], label->Bounding[1]);
		p->SetEnable(false);
		p->SetActive(true);
		//SetBoundingBoxActorEnable(p.get());
	};

	bool isFind = false;
	for (auto&& var : m_LabelActorList)
	{
		if (auto p = var.lock(); p && p->GetActive() == false
			&& p->GetRenderEngine() == view->GetRenderEngine())
		{
			isFind = true;
			func(p);

			break;
		}
	}

	if (!isFind)
	{
		auto actor = std::make_shared<BoundingBoxActor>();
		func(actor);
		view->GetRenderEngine()->GetSceneManager()->AddNodeToGlobleScene(actor);
		m_LabelActorList.emplace_back(actor);
	}
}
