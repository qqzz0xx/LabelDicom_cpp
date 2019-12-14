#include "BoundingBox3DLabelModel.h"
#include "BoundingBox3DActor.h"
#include "SliceView.h"
#include "AppMain.h"
#include "vtkBoundingBox.h"
#include "CoordHelper.h"
#include "CubeActor.h"
#include "BoxWidgetActor.h"
#include "LocalConfig.h"
#include "JsonHelper.h"
#include "GlobalVal.h"
#include "SliceCutterActor.h"
#include "RandomSliceBoxTools.h"

#include <QObject>
#include <QCheckBox>
#include <QTreeView>

BoundingBox3DLabelModel::BoundingBox3DLabelModel()
{
	m_Name = typeid(BoundingBox3DLabelModel*).name();
	m_Model = new QStandardItemModel;
	QStringList strHeader;
	strHeader << "ID" << "COLOR" << "DESC" << "VISIBLE";

	m_Model->setColumnCount(strHeader.size());
	m_Model->setHorizontalHeaderLabels(strHeader);
}

void BoundingBox3DLabelModel::Add(SliceView* view, std::shared_ptr<BoundingBox3DLabelItem> label)
{
	if (!m_IsDrawEnble) return;
	if (label->LabelType->ID.toInt() == 0) return;

	//auto d = label->Bounding[0] - label->Bounding[1];
	if (label->Bounds[1] - label->Bounds[0] < 5
		|| label->Bounds[3] - label->Bounds[2] < 5
		|| label->Bounds[5] - label->Bounds[4] < 5) return;

	m_LabelMap.emplace(QString::fromStdString(label->GetUUID()), label);

	auto p = AddActor(label);

	SetBoundingBoxActorEnable(p->Actors[0].lock().get());

	AddModelItem(label);

	gMainWindow->RenderAll();
}

void BoundingBox3DLabelModel::Add(SliceView* view, std::shared_ptr<RandomBoxLabelItem> label)
{
	m_RandomLabelMap.emplace(QString::fromStdString(label->GetUUID()), label);
	AddModelItem(label);
}

void BoundingBox3DLabelModel::Remove(const QString& id)
{
	RemoveRandomBox(id);
	RemoveBox3D(id);
}

void BoundingBox3DLabelModel::RemoveSelected()
{
	for (auto&& var : m_LabelActorList)
	{
		if (var->IsActive)
		{
			if (var->IsEnable)
			{
				RemoveBox3D(QString::fromStdString(var->GetUUID()));
				return;
			}
		}
	}
}

NRender::BoundingBoxActor* BoundingBox3DLabelModel::CheckPick(SliceView* view, const glm::dvec3& pos)
{
	for (auto&& var : m_LabelActorList)
	{
		if (var->IsActive)
		{
			for (size_t i = 0; i < 3; i++)
			{
				if (view->GetMprType() == (MprType)i)
				{
					auto actor = var->Actors[i];

					if (auto p = actor.lock(); p && p->GetActive() && p->CheckHitBox(pos))
					{
						return p.get();
					}
				}
			}
		}
	}

	return nullptr;
}

NRender::BoundingBoxActor* BoundingBox3DLabelModel::CheckContain(SliceView* view, const glm::dvec3& p1, const glm::dvec3& p2)
{
	vtkBoundingBox box;
	box.AddPoint((double*)glm::value_ptr(p1));
	box.AddPoint((double*)glm::value_ptr(p2));

	for (auto&& var : m_LabelActorList)
	{
		if (var->IsActive)
		{
			for (size_t i = 0; i < 3; i++)
			{
				if (view->GetMprType() == (MprType)i)
				{
					auto actor = var->Actors[i];

					if (auto p = actor.lock(); p && p->GetActive())
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
			}
		}
	}


	return nullptr;
}

void BoundingBox3DLabelModel::OnColorLabelUpdate()
{
	for (auto&& actors : m_LabelActorList)
	{
		for (auto&& var : actors->Actors)
		{
			if (auto p = var.lock(); p && p->GetActive())
			{
				p->UpdateColor();
			}
		}
	}
}

RandomBoxLabelItem* BoundingBox3DLabelModel::Find(const QString& id)
{
	/*auto it = m_LabelMap.find(id);
	if (it != m_LabelMap.end())
	{
		return it->second.get();
	}*/

	auto it1 = m_RandomLabelMap.find(id);
	if (it1 != m_RandomLabelMap.end())
	{
		return it1->second.get();
	}

	return nullptr;
}

void BoundingBox3DLabelModel::SetBoundingBoxActorEnable(NRender::BoundingBoxActor* actor)
{
	for (auto&& actors : m_LabelActorList)
	{
		if (actor&&actors->GetUUID() == actor->GetUUID())
		{
			actors->SetEnable(true);
			m_CurEnableID = actor->GetUUID();
		}
		else
		{
			actors->SetEnable(false);
		}
	}

	//gMainWindow->RenderAll();
}

void BoundingBox3DLabelModel::SetBoundingBoxActorEnable(const std::string& id)
{
	if (id.empty()) return;

	for (auto&& actors : m_LabelActorList)
	{
		if (actors->GetUUID() == id)
		{
			actors->SetEnable(true);
			m_CurEnableID = id;
		}
		else
		{
			actors->SetEnable(false);
		}
	}

	for (auto&& [k, v] : m_RandomLabelMap)
	{
		auto p = v->BoxActor.lock();
		if ( p )
		{
			if (p->GetUUID() == id)
			{
				p->SetSelected(true);
			}
			else
			{
				p->SetSelected(false);
			}
			
		}
	}

	auto rs = gMainWindow->GetRandomSlice(MprType::RandomZ);
	auto tools = rs->ui->view->GetTools()->Get<RandomSliceBoxTools>();
	if (tools)
	{
		tools->OnRotatePlanetToY();
	}
	//gMainWindow->RenderAll();
}

void BoundingBox3DLabelModel::OnFrameChanged(SliceView* view, int value)
{
	for (auto& p : m_LabelActorList)
	{
		p->SetActive(false);
	}

	for (auto&& [k, v] : m_LabelMap)
	{
		AddActor(v);
	}

	//SetBoundingBoxActorEnable(m_CurEnableID);
}

void BoundingBox3DLabelModel::Clear()
{
	m_LabelActorList.clear();
	m_LabelMap.clear();
}

void BoundingBox3DLabelModel::ComputeBounds(double* bounds, MprType ty, double* vp1, double* vp2)
{
	vtkBoundingBox box;
	box.AddPoint(vp1);
	box.AddPoint(vp2);

	switch (ty)
	{
	case MprType::Sagittal:
		vp1[0] = bounds[0];
		vp2[0] = bounds[1];
		break;
	case MprType::Coronal:
		vp1[1] = bounds[2];
		vp2[1] = bounds[3];
		break;
	case MprType::Axial:
		vp1[2] = bounds[4];
		vp2[2] = bounds[5];
		break;
	case MprType::None:

		break;
	default:
		break;
	}
	box.AddPoint(vp1);
	box.AddPoint(vp2);

	box.GetBounds(bounds);
}

void BoundingBox3DLabelModel::ExportJson(nlohmann::json& j_all)
{
	if (m_LabelMap.empty() && m_RandomLabelMap.empty()) return;

	//json j_all;
	for (auto&& [k, v] : m_LabelMap)
	{
		json j;
		JsonHelper::ArrayToJson(v->Bounds, 6, j["Bounds"]);
		j["Label"] = v->LabelType->ID.toStdString();
		j_all.push_back(j);
	}

	for (auto&& [k, v] : m_RandomLabelMap)
	{
		if (auto p = v->BoxActor.lock(); p)
		{
			json j;
			j["Label"] = v->LabelType->ID.toStdString();
			JsonHelper::ArrayToJson(v->Bounds, 6, j["Bounds"]);
			JsonHelper::ArrayToJson(p->GetMatrix()->Element[0], 16, j["Matrix"]);
			j_all.push_back(j);
		}
	}

	//auto path_str = path.toStdString();
	//LocalConfig conf;
	//conf.SetRootPath(path_str);
	//std::stringstream ss;
	//ss << g_GlobalVal.FileName.toStdString() << "_Bounds.json";
	//conf.SetFileName(ss.str());
	//conf.SetJsonData(j_all);
	//conf.Save();
}

BoundingBox3DActorItem* BoundingBox3DLabelModel::AddActor(std::shared_ptr<BoundingBox3DLabelItem> label)
{
	BoundingBox3DActorItem* pItem;

	for (size_t i = 0; i < 6; i++)
	{
		label->Bounds[i] = label->Bounds[i];
	}

	auto mpr = gMainWindow->GetMprComponent();

	bool isFind = false;
	for (auto&& var : m_LabelActorList)
	{
		if (!var->IsActive)
		{
			isFind = true;
			
			var->SetActive(true);

			for (size_t i = 0; i < 3; i++)
			{
				if (auto p = var->Actors[i].lock(); p)
				{
					InitActor(i, p, label, var);
				}
			}
			pItem = var.get();
			break;
		}
	}

	if (!isFind)
	{
		auto item = std::make_shared<BoundingBox3DActorItem>();
		for (size_t i = 0; i < 3; i++)
		{
			auto actor = std::make_shared<BoundingBox3DActor>();
			InitActor(i, actor, label, item);
			auto sc = mpr->GetSliceComponent((MprType)i);
			sc->GetSliceView()->GetRenderEngine()
				->GetSceneManager()->AddNodeToGlobleScene(actor);

			item->Actors[i] = actor;
		}
		item->SetActive(true);
		pItem = item.get();

		m_LabelActorList.emplace_back(item);
	}

	pItem->InitVoxBox(label);
	pItem->SetUUID(label->GetUUID());

	return pItem;

}

void BoundingBox3DLabelModel::AddVoxCube(BoundingBox3DActorItem* item, std::shared_ptr<BoundingBox3DLabelItem> label)
{
	
}

void BoundingBox3DLabelModel::InitActor(int i, std::shared_ptr<BoundingBox3DActor> p,
	std::shared_ptr<BoundingBox3DLabelItem> label, std::shared_ptr<BoundingBox3DActorItem> actorItem)
{
	auto mpr = gMainWindow->GetMprComponent();

	auto sc = mpr->GetSliceComponent((MprType)i);
	auto view = sc->GetSliceView();
	double p1[3] = { label->Bounds[0],label->Bounds[2],label->Bounds[4] };
	double p2[3] = { label->Bounds[1],label->Bounds[3],label->Bounds[5] };
	double wp1[3], wp2[3];
	CoordHelper::VoxToWorldPos(p1, view->GetSingleImageActor(), sc->GetReslice()->GetResliceMatrix(), wp1);
	CoordHelper::VoxToWorldPos(p2, view->GetSingleImageActor(), sc->GetReslice()->GetResliceMatrix(), wp2);

	wp1[2] = wp2[2] = 1;

	p->Init(label);
	p->SetUUID(label->GetUUID());
	p->InitPoints(glm::make_vec3(wp1), glm::make_vec3(wp2));
	p->SetEnable(false);

	p->ChangedCallback = [&, i, sc, view, label](BoundingBox3DActor* actor)
	{
		auto p1 = actor->m_Points[0];
		auto p2 = actor->m_Points[1];
		double vp1[3], vp2[3];
		CoordHelper::WorldToVoxPos(glm::value_ptr(p1), view->GetSingleImageActor(), sc->GetReslice()->GetResliceMatrix(), vp1);
		CoordHelper::WorldToVoxPos(glm::value_ptr(p2), view->GetSingleImageActor(), sc->GetReslice()->GetResliceMatrix(), vp2);

		ComputeBounds(label->Bounds, (MprType)i, vp1, vp2);
		for (auto&& var : m_LabelActorList)
		{
			if (var->GetUUID() == label->GetUUID())
			{
				for (size_t j = 0; j < 3; j++)
				{
					if (j != i)
					{
						auto p = var->Actors[j].lock();
						double wp1[3], wp2[3];
						CoordHelper::VoxToWorldPos(vp1, (MprType)j, wp1);
						CoordHelper::VoxToWorldPos(vp2, (MprType)j, wp2);
						wp2[2] = wp1[2] = 1;
						p->InitPoints(glm::make_vec3(wp1), glm::make_vec3(wp2));
					}
				}
				//auto mpr = gMainWindow->GetMprComponent();
				//auto sc = mpr->GetSliceComponent((MprType)i);
				//auto view = sc->GetSliceView();
				//OnFrameChanged(view, sc->GetSliderValue());
				//var->SetActive(false);
				var->InitVoxBox(label);
			}

		}

		gMainWindow->RenderAll();
	};

	auto val = sc->GetSliderValue();
	int min = label->Bounds[i * 2];
	int max = label->Bounds[i * 2 + 1];

	if (val >= min && val <= max)
	{
		p->SetActive(true);
	}
	else
	{
		p->SetActive(false);
	}

}

void BoundingBox3DLabelModel::AddModelItem(std::shared_ptr<BoundingBox3DLabelItem> label)
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

	auto uq = new QStandardItemUUID;
	uq->uuid = label->GetUUID();
	m_Model->setItem(row, 3, uq);

	auto button = new QCheckBox(gMainWindow);
	button->setChecked(true);

	auto view = gMainWindow->m_LabelListComponent->ui->treeView_2;
	view->setIndexWidget(m_Model->indexFromItem(uq), button);

	label->StandardItem = uq;

	QObject::connect(button, &QCheckBox::clicked, [&, id = label->GetUUID()](bool b) {

		for (auto&& actors : m_LabelActorList)
		{
			if (actors->GetUUID() != id) continue;
			if (auto p = actors->VoxCubeActor.lock(); p )
			{
				p->SetActive(b);
			}
			
			for (auto&& var : actors->Actors)
			{
				if (auto p = var.lock(); p)
				{
					p->SetActive(b);
				}
			}
			
		}

		auto it = m_RandomLabelMap.find(QString::fromStdString(id));
		if (it != m_RandomLabelMap.end())
		{
			if (auto p = it->second->BoxActor.lock(); p)
			{
				p->SetActive(b);
			}

			for (auto&& var : it->second->CutterActors)
			{
				if (auto p = var.lock(); p)
				{
					p->SetActive(b);
				}
			}

			gMainWindow->ForEachRandomSlice([](auto s) {
				s->ui->view->Render();
			});
		}

		gMainWindow->RenderAll();
	});

}

void BoundingBox3DLabelModel::RemoveModelItem(QStandardItem* item)
{
	auto idx = m_Model->indexFromItem(item);
	if (idx.isValid())
	{
		m_Model->removeRow(idx.row());
	}
}

void BoundingBox3DLabelModel::RemoveBox3D(const QString& id)
{
	auto it = m_LabelMap.find(id);
	auto s_id = id.toStdString();
	if (it != m_LabelMap.end())
	{
		RemoveModelItem(it->second->StandardItem);
		m_LabelMap.erase(id);

		for (auto&& var : m_LabelActorList)
		{
			if (var->GetUUID() == s_id)
			{
				var->SetEnable(false);
				var->SetActive(false);
			}
		}

		gMainWindow->RenderAll();
	}
}

void BoundingBox3DLabelModel::RemoveRandomBox(const QString& id)
{
	auto it = m_RandomLabelMap.find(id);
	auto s_id = id.toStdString();
	if (it != m_RandomLabelMap.end())
	{
		auto volume = gMainWindow->GetMprComponent()->GetVolumeComponent();
		auto mgr = volume->GetRenderEngine()->GetSceneManager();
		mgr->RemoveNodeByID(it->first.toStdString());

		gMainWindow->ForEachRandomSlice([&](auto s) {
			auto mgr1 = s->ui->view->GetRenderEngine()->GetSceneManager();
			mgr1->RemoveNodeByID(it->first.toStdString());

			s->ui->view->Render();
		});
	
		RemoveModelItem(it->second->StandardItem);

		m_RandomLabelMap.erase(id);

		gMainWindow->RenderAll();
	}
}

void BoundingBox3DActorItem::SetActive(bool v)
{
	for (auto&& actor : Actors)
	{
		if (auto p = actor.lock(); p != nullptr)
		{
			p->SetActive(v);
		}
	}

	if (auto p = VoxCubeActor.lock(); p)
	{
		p->SetActive(v);
	}

	IsActive = v;
}

void BoundingBox3DActorItem::SetEnable(bool v)
{
	for (auto&& actor : Actors)
	{
		if (auto p = actor.lock(); p != nullptr)
		{
			p->SetEnable(v);
		}

	}

	if (auto p = VoxCubeActor.lock(); p)
	{
		p->SetActive(v);
	}

	IsEnable = v;
}

void BoundingBox3DActorItem::InitVoxBox(std::shared_ptr<BoundingBox3DLabelItem> label)
{
	auto volume = gMainWindow->GetMprComponent()->GetVolumeComponent();

	if (VoxCubeActor.lock() == nullptr)
	{
		auto cube = std::make_shared<BoxWidgetActor>();
		auto mgr = volume->GetRenderEngine()->GetSceneManager();
		mgr->AddNodeToGlobleScene(cube);
		VoxCubeActor = cube;
	}
	

	double center[3];
	auto bds = label->Bounds;
	center[0] = (bds[0] + bds[1]) * 0.5;
	center[1] = (bds[2] + bds[3]) * 0.5;
	center[2] = (bds[4] + bds[5]) * 0.5;

	if (auto p = VoxCubeActor.lock(); p)
	{
		/*	p->SetPosition(center[0], center[1], center[2]);
			p->SetXYZLength(bds[1] - bds[0], bds[3] - bds[2], bds[5] - bds[4]);
			double rgba[4];
			QtHelper::QColorToVTKRGBA(label->LabelType->Color, rgba);
			p->SetRGBA(rgba);*/

		p->SetBounds(bds);
		p->SetEnable(false);

		volume->GetRenderEngine()->Render();
	}


}
