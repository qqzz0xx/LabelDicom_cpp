#include "BoxWidgetActor.h"
#include "NCommand.h"
#include "RenderEngine.h"
#include "vtkBoxRepresentation.h"
NRender::BoxWidgetActor::BoxWidgetActor()
{
	m_Widget = vtkSmartPointer<vtkBoxWidget2>::New();
	m_Widget->GetRepresentation()->SetPlaceFactor(1);
	m_Matrix = vtkSmartPointer<vtkMatrix4x4>::New();

	auto func = [&](auto ev)
	{
		auto cmd = vtkSmartPointer<NCommand>::New();
		cmd->Callback = [this, ev](auto p)
		{
			OnInteractor((int)ev);
		};
		m_Widget->AddObserver(ev, cmd);
	};


	func(vtkCommand::StartInteractionEvent);
	func(vtkCommand::EndInteractionEvent);
	func(vtkCommand::InteractionEvent);

	auto cmd1 = vtkSmartPointer<NCommand>::New();
	cmd1->Callback =[&](auto p)
	{
		auto trans = vtkSmartPointer<vtkTransform>::New();
		GetRepresentation()->GetTransform(trans);
		
		m_Matrix->DeepCopy(trans->GetMatrix());
	};
	m_Widget->AddObserver(vtkCommand::InteractionEvent, cmd1);
}

vtkBoxRepresentation* NRender::BoxWidgetActor::GetRepresentation()
{
	return vtkBoxRepresentation::SafeDownCast(m_Widget->GetRepresentation());
}

void NRender::BoxWidgetActor::SetRGBA(double* rgba)
{
	auto func = [&](auto p)
	{
		p->SetColor(rgba);
		p->SetOpacity(rgba[3]);
	};

	func(GetRepresentation()->GetOutlineProperty());
}

void NRender::BoxWidgetActor::SetBounds(double* bds)
{
	if (m_Widget)
	{
		m_Widget->GetRepresentation()->PlaceWidget(bds);
	}
}

void NRender::BoxWidgetActor::SetTransform(vtkTransform* trans)
{
	GetRepresentation()->SetTransform(trans);
	m_Matrix->DeepCopy(trans->GetMatrix());
}

void NRender::BoxWidgetActor::SetSelected(bool v)
{
	if (m_Widget)
	{
		m_Widget->SetMoveFacesEnabled(v);
		m_Widget->SetRotationEnabled(v);

		GetRepresentation()->SetOutlineCursorWires(v);

		if (v)
		{
			GetRepresentation()->HandlesOn();
		}
		else
		{
			GetRepresentation()->HandlesOff();
		}

		auto enable = m_Widget->GetEnabled();
		if (enable)
		{
			m_Widget->SetEnabled(!enable);
			m_Widget->SetEnabled(enable);
		}
	}
}

void NRender::BoxWidgetActor::Start()
{
	__super::Start();
	if (m_RenderEngine)
	{
		m_Widget->SetInteractor(m_RenderEngine->GetInteractor());
		m_Widget->On();
	}
}

void NRender::BoxWidgetActor::Exit()
{
	__super::Exit();
	if (m_RenderEngine)
	{
		m_Widget->SetInteractor(m_RenderEngine->GetInteractor());
		m_Widget->Off();
	}
}

void NRender::BoxWidgetActor::SetActive(bool isShow)
{
	__super::SetActive(isShow);

	if (m_Widget)
	{
		m_Widget->SetEnabled(isShow);
	}
}

void NRender::BoxWidgetActor::SetEnable(bool isEnable)
{
	__super::SetEnable(isEnable);

	if (m_Widget)
	{
		m_Widget->SetRotationEnabled(isEnable);
		m_Widget->SetScalingEnabled(isEnable);
		m_Widget->SetTranslationEnabled(isEnable);
		m_Widget->SetMoveFacesEnabled(isEnable);

		auto rep = GetRepresentation();
		rep->SetOutlineCursorWires(isEnable);
		if (isEnable)
		{
			rep->HandlesOn();
		}
		else
		{
			rep->HandlesOff();
		}
	}
}

double* NRender::BoxWidgetActor::GetCenter()
{
	if (m_Widget)
	{
		auto bds = GetRepresentation()->GetBounds();

		m_Center[0] = (bds[0] + bds[1]) * 0.5;
		m_Center[1] = (bds[2] + bds[3]) * 0.5;
		m_Center[2] = (bds[4] + bds[5]) * 0.5;
	}
	return m_Center;
}

void NRender::BoxWidgetActor::OnInteractor(int ev)
{
	if (ev == vtkCommand::InteractionEvent)
	{
		auto t = vtkSmartPointer<vtkTransform>::New();
		GetRepresentation()->GetTransform(t);
		m_Matrix->DeepCopy(t->GetMatrix());
		if (MoveCallback) MoveCallback();
	}
	else if (ev == vtkCommand::StartInteractionEvent)
	{
		if (StartMoveCallback) StartMoveCallback();
	}
	else if (ev == vtkCommand::EndInteractionEvent)
	{
		if (EndMoveCallback) EndMoveCallback();
	}
}
