#include "BoundingBox3DTools.h"
#include "ModelManager.h"
#include "MprPositionModel.h"
#include "AppMain.h"
#include "BoundingBox3DLabelModel.h"
#include "MainWindow.h"
#include "CoordHelper.h"
#include "MprComponent.h"
#include "vtkBoundingBox.h"
#include "LoadMgr.h"

void BoundingBox3DTools::OnMouseEvent(MouseEventType type)
{

	auto model = gModelMgr->GetModel<MprPositionModel>();
	auto p = glm::make_vec3(model->m_WorldPos);
	p.z = 1;

	auto boxmodel = gModelMgr->GetModel<BoundingBox3DLabelModel>();

	bool isChanged = false;

	if (type == MouseEventType_LeftDown)
	{
		if (m_State == State::Start)
		{
			m_BoxPoints[0] = p;
			m_BoxPoints[1] = p;
			m_State = State::Drag;
		}
		else if (m_State == State::Editor)
		{
			if (boxmodel->CheckPick(m_View, p) == nullptr)
			{
				boxmodel->SetBoundingBoxActorEnable(nullptr);
				isChanged = true;

				m_BoxPoints[0] = p;
				m_BoxPoints[1] = p;
				m_State = State::Drag;
			}
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
			//m_State = State::Start;
		}

	}
	else if (type == MouseEventType_LeftUp)
	{
		if (m_State == State::Drag)
		{
			auto p1 = m_BoxPoints[0];
			auto p2 = m_BoxPoints[1];
			if (auto hit = boxmodel->CheckContain(m_View, p1, p2); hit != nullptr)
			{
				boxmodel->SetBoundingBoxActorEnable(hit);
				m_State = State::Editor;
			}
			else
			{
				auto item = std::make_shared<BoundingBox3DLabelItem>();


				auto sc = gMainWindow->GetMprComponent()->GetSliceComponent(m_View->GetMprType());
				double vp1[3], vp2[3];
				CoordHelper::WorldToVoxPos(glm::value_ptr(p1), m_View->GetSingleImageActor(),
					sc->GetReslice()->GetResliceMatrix(), vp1);
				CoordHelper::WorldToVoxPos(glm::value_ptr(p2), m_View->GetSingleImageActor(),
					sc->GetReslice()->GetResliceMatrix(), vp2);

				//auto rotMat = vtkSmartPointer<vtkMatrix4x4>::New();
				//rotMat->DeepCopy(sc->GetReslice()->GetResliceMatrix());
				//rotMat->SetElement(0, 3, 0);
				//rotMat->SetElement(1, 3, 0);
				//rotMat->SetElement(2, 3, 0);

				//auto transform = vtkSmartPointer<vtkTransform>::New();
				//transform->SetMatrix(rotMat);
				//transform->Update();
				//auto tvp1 = transform->TransformDoublePoint(vp1);
				//auto tvp2 = transform->TransformDoublePoint(vp2);
				//memcpy(vp1, tvp1, sizeof(double) * 3);
				//memcpy(vp2, tvp2, sizeof(double) * 3);

				auto dim = gLoadMgr->GetDims();
				auto mprType = m_View->GetMprType();
				vtkBoundingBox box;
				box.AddPoint(vp1);
				box.AddPoint(vp2);

				switch (mprType)
				{
				case MprType::Sagittal:
					vp1[0] = 0;
					vp2[0] = dim[0] - 1;
					break;
				case MprType::Coronal:
					vp1[1] = 0;
					vp2[1] = dim[1] - 1;
					break;
				case MprType::Axial:
					vp1[2] = 0;
					vp2[2] = dim[2] - 1;
					break;
				case MprType::None:
				
					break;
				default:
					break;
				}
				box.AddPoint(vp1);
				box.AddPoint(vp2);

				box.GetBounds(item->Bounds);

				item->LabelType = gMainWindow->GetCurLabelTypeItem();

				boxmodel->Add(m_View, item);
				m_State = State::Editor;
			}

			m_RectangleActor->SetActive(false);
			isChanged = true;

		}

	}

	if (isChanged)
	{
		m_View->GetRenderEngine()->Render();
	}
}

void BoundingBox3DTools::SetEnable(bool v)
{
	__super::SetEnable(v);

	if (v)
	{
		if (m_RectangleActor == nullptr)
		{
			m_RectangleActor = std::make_shared<RectangleActor>();
			m_RectangleActor->SetActive(false);
			m_View->GetRenderEngine()->GetSceneManager()->AddNodeToGlobleScene(m_RectangleActor);
		}
		m_State = State::Start;
	}
	else
	{
		auto boxmodel = gModelMgr->GetModel<BoundingBox3DLabelModel>();
		boxmodel->SetBoundingBoxActorEnable(nullptr);

		m_State = State::None;
	}
}
