#include "LandMarkTools.h"
#include "LandMarkActor.h"
#include "SliceComponent.h"
#include "RenderEngine.h"
#include "AppMain.h"
#include "ModelManager.h"
#include "MprPositionModel.h"
#include "LabelSelectDialog.h"
#include "QtHelper.h"
#include "LandMarkListModel.h"
#include "LineActor.h"
using namespace NRender;

LandMarkTools::LandMarkTools()
{
	m_Enable = false;
}

void LandMarkTools::OnMouseEvent(MouseEventType type)
{
	if (type == MouseEventType_LeftDown)
	{
		auto model = gModelMgr->GetModel<MprPositionModel>();
		auto model1 = gModelMgr->GetModel<LandMarkListModel>();

		m_PickedActor = model1->CheckPickActor(model->m_WorldPos);
		auto inTranslation = m_View->ui->translationPoints->isChecked();

		if (!inTranslation)
		{
			auto labelType = gMainWindow->GetCurLabelType();
			if (labelType == 0) return;


			auto labelModel = gModelMgr->GetModel<ColorLabelTableModel>();
			auto label = labelModel->GetColorLabel(QString::number(labelType));

			auto it = std::make_shared<LandMarkListItem>();
			it->ImageIndex = m_CurIndex;
			it->LabelType = label;
			it->Pos.x = model->m_ImageXY[0];
			it->Pos.y = model->m_ImageXY[1];
			it->ViewType = m_View->GetMprType();

			model1->Add(m_View, it);
			model1->UpdateLines(m_View);

			m_View->GetRenderEngine()->Render();
		}
	}
	//else if (type == MouseEventType_Move)
	//{
	//	if (auto p = m_CurLandMarkActor.lock(); p && p->GetActive())
	//	{
	//		auto p1 = p->GetActor()->GetPosition();
	//		auto p2 = m_View->GetRenderEngine()->GetEventWorldPos();
	//		p1[2] = p2[2] = 1;

	//		if (auto p = m_CurLineActor.lock(); p)
	//		{
	//			p->SetPos1(p1);
	//			p->SetPos2(p2);
	//			p->SetActive(true);
	//			m_View->Render();
	//		}
	//	}
	//	else
	//	{
	//		if (auto p = m_CurLineActor.lock(); p)
	//		{
	//			p->SetActive(false);
	//			m_View->Render();
	//		}
	//	}
	//}
	else if (type == MouseEventType_LeftUp)
	{
		m_PickedActor = nullptr;
	}
	else if (IsLeftDrag(type))
	{
		auto inTranslation = m_View->ui->translationPoints->isChecked();

		if (m_PickedActor != nullptr && inTranslation)
		{
			auto model = gModelMgr->GetModel<MprPositionModel>();
			auto model1 = gModelMgr->GetModel<LandMarkListModel>();
			auto pos = m_View->GetRenderEngine()->GetEventWorldPos();
			m_PickedActor->SetPosition(pos);

			auto label = model1->Find(QString::fromStdString(m_PickedActor->GetUUID()));
			
			label->Pos.x = model->m_ImageXY[0];
			label->Pos.y = model->m_ImageXY[1];

			model1->UpdateLines(m_View);

			m_View->GetRenderEngine()->Render();
		}
	}
}

void LandMarkTools::SetEnable(bool v)
{
	__super::SetEnable(v);
	if (v)
	{
		if (m_CurLineActor.lock() == nullptr)
		{
			auto p = std::make_shared<LineActor>();
			p->SetActive(false);
			double rgba[4] = { 1,0.5,0,0.6 };
			p->SetColor(rgba);
			p->SetAlpha(rgba[3]);
			m_View->GetRenderEngine()->GetSceneManager()->AddNodeToGlobleScene(p);

			m_CurLineActor = p;
		}

		m_View->ui->stackedWidget->setCurrentWidget(m_View->ui->pageLandMark);
	}
	else
	{
		m_View->ui->stackedWidget->setCurrentWidget(m_View->ui->pageNone);
		if (auto p = m_CurLineActor.lock(); p)
		{
			p->SetActive(false);
		}
	}
}

void LandMarkTools::SetCurIndex(int index)
{
	__super::SetCurIndex(index);
	if (auto p = m_CurLineActor.lock(); p)
	{
		p->SetActive(false);
	}
}

