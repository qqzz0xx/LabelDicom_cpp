#include "RandomSliceBoxTools.h"
#include "ModelManager.h"
#include "MprPositionModel.h"
#include "AppMain.h"
#include "BoundingBox3DLabelModel.h"
#include "MainWindow.h"
#include "CoordHelper.h"
#include "MprComponent.h"
#include "vtkBoundingBox.h"
#include "BoxWidgetActor.h"
#include "vtkBoxRepresentation.h"
#include "SliceCutterActor.h"
#include "vtkCubeSource.h"
#include "QtHelper.h"


std::shared_ptr<RandomBoxLabelItem> RandomSliceBoxTools::m_LabelItem = nullptr;

void RandomSliceBoxTools::OnMouseEvent(MouseEventType type)
{
	auto evPos = m_View->GetRenderEngine()->GetEventWorldPos();
	auto p = glm::dvec3(evPos[0], evPos[1], 1);

	//auto boxmodel = gModelMgr->GetModel<BoundingBox3DLabelModel>();

	bool isChanged = false;

	if (type == MouseEventType_LeftDown)
	{
		if (m_State == State::Start)
		{
			m_BoxPoints[0] = p;
			m_BoxPoints[1] = p;
			m_State = State::Drag;
		}

	}
	else if (type == MouseEventType_Move)
	{
		if (m_State == State::Drag)
		{
			m_BoxPoints[1] = p;
			m_RectangleActor->SetPoints(m_BoxPoints[0], m_BoxPoints[1]);
			m_RectangleActor->SetActive(true);

			isChanged = true;
		}

	}
	else if (type == MouseEventType_LeftUp)
	{
		if (m_State == State::Drag)
		{
			auto p1 = m_BoxPoints[0];
			auto p2 = m_BoxPoints[1];
			p1.z = p2.z = 0;
			auto d = glm::length(p1 - p2);

			auto labelTypeID = gMainWindow->GetCurLabelType();
			
			if (d < 5 || labelTypeID == 0)
			{
				m_RectangleActor->SetActive(false);
				m_View->GetRenderEngine()->Render();
				m_State = State::Start;
				return;
			}
			
			auto volume = gMainWindow->GetMprComponent()->GetVolumeComponent();
			auto mgr = volume->GetRenderEngine()->GetSceneManager();

			if (m_LabelItem == nullptr)
			{
				m_LabelItem = std::make_shared<RandomBoxLabelItem>();
				auto boxActor = std::make_shared<BoxWidgetActor>();
				m_LabelItem->BoxActor = boxActor;

				boxActor->SetUUID(m_LabelItem->GetUUID());
				auto m_Widget = boxActor->m_Widget;
				m_Widget->SetRotationEnabled(false);
				m_Widget->SetScalingEnabled(false);
				m_Widget->SetTranslationEnabled(false);
				m_Widget->SetMoveFacesEnabled(true);

				gMainWindow->ForEachRandomSlice([&](RandomSlice* s) {
					auto cutter = std::make_shared<SliceCutterActor>();
					cutter->SetUUID(m_LabelItem->GetUUID());
					auto src = vtkCubeSource::New();
					cutter->InitSource(src);
					cutter->SetReslice(s->GetReslice()->GetImageReslice());
					s->ui->view->GetRenderEngine()->GetSceneManager()->AddNodeToGlobleScene(cutter);
					m_LabelItem->CutterActors.emplace_back(cutter);
				});
		

				boxActor->MoveCallback = [&, p = m_LabelItem.get()]()
				{
					//p->Refresh();
					for (auto&& actor : p->CutterActors)
					{
						if (auto p = actor.lock(); p)
						{
							p->Refresh();
						}
					}
				
					gMainWindow->ForEachRandomSlice([](auto s) {s->ui->view->Render(); });
				};

				mgr->AddNodeToGlobleScene(boxActor);


				//m_LabelItem->BoxActor = boxActor;
				//m_LabelItem->CutterActor = cutter;
			}

			auto sc = gMainWindow->GetRandomSlice(m_View->GetMprType());

			double vp1[3] = { p1.x, p1.y, 50 };
			double vp2[3] = { p2.x, p2.y, -50 };

			auto rotMat = vtkSmartPointer<vtkMatrix4x4>::New();
			rotMat->DeepCopy(sc->GetReslice()->GetResliceMatrix());

			auto transform = vtkSmartPointer<vtkTransform>::New();
			transform->SetMatrix(rotMat);
			transform->Update();

			LOG(INFO) << "vp1:" << vp1[0] << " " << vp1[1] << " " << vp1[2];
			LOG(INFO) << "vp2:" << vp2[0] << " " << vp2[1] << " " << vp2[2];

			vtkBoundingBox box;
			box.AddPoint(vp1);
			box.AddPoint(vp2);
			box.GetBounds(m_LabelItem->Bounds);

			m_LabelItem->LabelType = gMainWindow->GetCurLabelTypeItem();

			auto boxActor = m_LabelItem->BoxActor.lock();

			boxActor->SetBounds(m_LabelItem->Bounds);
			boxActor->SetTransform(transform);
			double rgba[4];
			QtHelper::QColorToVTKRGBA(m_LabelItem->LabelType->Color, rgba);
			boxActor->SetRGBA(rgba);

			for (auto&& var : m_LabelItem->CutterActors)
			{
				if (auto cutter = var.lock(); cutter)
				{
					cutter->SetColor(rgba);
					cutter->SetAlpha(rgba[3]);
					auto src = cutter->GetSource<vtkCubeSource>();
					src->SetBounds(m_LabelItem->Bounds);
					cutter->SetVoxMat(boxActor->GetMatrix());
					cutter->Refresh();
				}
			}
		
			auto bds = m_LabelItem->Bounds;
			qDebug() << "Origin Bds:" << bds[0] << " " << bds[1] << " " << bds[2] << " " << bds[3]
				<< " " << bds[4] << " " << bds[5];

			volume->Render();

			m_State = State::Start;
		}

		m_RectangleActor->SetActive(false);
		isChanged = true;
	}

	if (isChanged)
	{
		gMainWindow->ForEachRandomSlice([](auto s) {
			s->ui->view->GetRenderEngine()->Render();
		});
	}
}

void RandomSliceBoxTools::SetEnable(bool v)
{
	m_Enable = v;

	if (v)
	{
		if (m_RectangleActor == nullptr)
		{
			m_RectangleActor = std::make_shared<RectangleActor>();
			m_RectangleActor->SetActive(false);
			m_View->GetRenderEngine()->GetSceneManager()->AddNodeToGlobleScene(m_RectangleActor);
		}
		m_View->ui->stackedWidget->setCurrentWidget(m_View->ui->pageRandom);
		m_State = State::Start;
	}
	else
	{
		m_View->ui->stackedWidget->setCurrentWidget(m_View->ui->pageNone);
		m_State = State::None;
	}
}

void RandomSliceBoxTools::OnAccpet()
{
	if (m_LabelItem == nullptr) return;

	auto model = gModelMgr->GetModel<BoundingBox3DLabelModel>();
	model->Add(m_View, m_LabelItem);
	//model->SetBoundingBoxActorEnable(m_LabelItem->GetUUID());
	m_LabelItem->BoxActor.lock()->SetSelected(false);

	m_LabelItem = nullptr;
}

void RandomSliceBoxTools::OnRotatePlanetToY()
{
	RandomBoxLabelItem* box;
	box = m_LabelItem.get();

	if (m_LabelItem == nullptr)
	{
		auto model = gModelMgr->GetModel<BoundingBox3DLabelModel>();
		auto id = gMainWindow->m_LabelListComponent->GetSelectedBoxID();
		box = model->Find(id);
	}

	if (box == nullptr)return;

	auto volume = gMainWindow->GetMprComponent()->GetVolumeComponent();
	auto pw = volume->GetPlaneWidget();
	auto rep = (vtkImplicitPlaneRepresentation*)pw->GetRepresentation();
	auto mat = box->BoxActor.lock()->GetMatrix();

	rep->SetNormal(mat->Element[0][2], mat->Element[1][2], mat->Element[2][2]);
	rep->SetOrigin(box->BoxActor.lock()->GetCenter());

	auto plane = vtkSmartPointer<vtkPlane>::New();
	rep->GetPlane(plane);


	gMainWindow->ForEachRandomSlice([plane](auto slice) {
		slice->UpdateResliceByPlane(plane);
		slice->ui->view->Render();
	});


	volume->Render();
}

void RandomSliceBoxTools::OnRotatePlanetToZ()
{
	RandomBoxLabelItem* box;
	box = m_LabelItem.get();

	if (m_LabelItem == nullptr)
	{
		auto model = gModelMgr->GetModel<BoundingBox3DLabelModel>();
		auto id = gMainWindow->m_LabelListComponent->GetSelectedBoxID();
		box = model->Find(id);
	}

	if (box == nullptr)return;


	auto volume = gMainWindow->GetMprComponent()->GetVolumeComponent();
	auto pw = volume->GetPlaneWidget();
	auto rep = (vtkImplicitPlaneRepresentation*)pw->GetRepresentation();
	auto mat = box->BoxActor.lock()->GetMatrix();

	rep->SetNormal(mat->Element[0][1], mat->Element[1][1], mat->Element[2][1]);
	rep->SetOrigin(box->BoxActor.lock()->GetCenter());

	auto plane = vtkSmartPointer<vtkPlane>::New();
	rep->GetPlane(plane);


	gMainWindow->ForEachRandomSlice([plane](auto slice) {
		slice->UpdateResliceByPlane(plane);
		slice->ui->view->Render();
	});

	volume->Render();
}

void RandomSliceBoxTools::OnClear()
{
	if (m_LabelItem == nullptr) return;

	auto volume = gMainWindow->GetMprComponent()->GetVolumeComponent();
	auto mgr = volume->GetRenderEngine()->GetSceneManager();
	mgr->RemoveNodeByID(m_LabelItem->GetUUID());

	gMainWindow->ForEachRandomSlice([&](auto s) {
		auto mgr1 = s->ui->view->GetRenderEngine()->GetSceneManager();
		mgr1->RemoveNodeByID(m_LabelItem->GetUUID());

		s->ui->view->Render();
	});

	volume->Render();

	m_LabelItem = nullptr;
}

void RandomSliceBoxTools::OnRotatePlanetToX()
{
	RandomBoxLabelItem* box;
	box = m_LabelItem.get();

	if (m_LabelItem == nullptr)
	{
		auto model = gModelMgr->GetModel<BoundingBox3DLabelModel>();
		auto id = gMainWindow->m_LabelListComponent->GetSelectedBoxID();
		box = model->Find(id);
	}

	if (box == nullptr)return;
	

	auto volume = gMainWindow->GetMprComponent()->GetVolumeComponent();
	auto pw = volume->GetPlaneWidget();
	auto rep = (vtkImplicitPlaneRepresentation*)pw->GetRepresentation();
	auto mat = box->BoxActor.lock()->GetMatrix();

	rep->SetNormal(mat->Element[0][0], mat->Element[1][0], mat->Element[2][0]);
	rep->SetOrigin(box->BoxActor.lock()->GetCenter());

	auto plane = vtkSmartPointer<vtkPlane>::New();
	rep->GetPlane(plane);

	gMainWindow->ForEachRandomSlice([plane](auto slice) {
		slice->UpdateResliceByPlane(plane);
		slice->ui->view->Render();
	});

	volume->Render();
}
