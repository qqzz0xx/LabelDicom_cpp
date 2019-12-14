#include "BoundingBoxTools.h"
#include "ModelManager.h"
#include "MprPositionModel.h"
#include "AppMain.h"
#include "BoundingBoxLabelModel.h"
#include "MainWindow.h"

void BoundingBoxTools::OnMouseEvent(MouseEventType type)
{
	auto model = gModelMgr->GetModel<MprPositionModel>();
	auto p = glm::make_vec3(model->m_WorldPos);
	p.z = 1;
	
	auto boxmodel = gModelMgr->GetModel<BoundingBoxLabelModel>();

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

			boxmodel = gModelMgr->GetModel<BoundingBoxLabelModel>();
			auto p1 = m_BoxPoints[0];
			auto p2 = m_BoxPoints[1];
			if (auto hit = boxmodel->CheckContain(m_View, p1, p2); hit != nullptr)
			{
				boxmodel->SetBoundingBoxActorEnable(hit);
				m_State = State::Editor;
			}
			else
			{
				auto item = std::make_shared<BoundingBoxLabelItem>();
				item->Bounding[0] = p1;
				item->Bounding[1] = p2;
				item->SliceType = m_View->GetMprType();
				item->FrameCount = m_CurIndex;
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

void BoundingBoxTools::SetEnable(bool v)
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
		auto boxmodel = gModelMgr->GetModel<BoundingBoxLabelModel>();
		boxmodel->SetBoundingBoxActorEnable(nullptr);

		m_State = State::None;
	}
}
