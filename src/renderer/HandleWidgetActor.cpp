#include "HandleWidgetActor.h"
#include "RenderEngine.h"
#include "NCommand.h"
#include "vtkProperty2D.h"

NRender::HandleWidgetActor::HandleWidgetActor()
{
	m_Name = "HandleWidgetActor";
	m_Handle = vtkSmartPointer<vtkHandleWidget>::New();
	m_HandleRep = vtkSmartPointer<vtkPointHandleRepresentation2D>::New();

	m_HandleRep->BuildRepresentation();
	m_HandleRep->GetProperty()->SetColor(0, 1, 0);
	m_Handle->SetRepresentation(m_HandleRep);

	auto func = [&](auto ev, auto f)
	{
		auto cmd = vtkSmartPointer<NCommand>::New();
		cmd->Callback = [&](auto p)
		{
			if (f)
			{
				auto pos = m_HandleRep->GetWorldPosition();
				f(pos);
			}
		};
		m_Handle->AddObserver(ev, cmd);
	};

	func(vtkCommand::StartInteractionEvent, StartMoveCallback);
	func(vtkCommand::EndInteractionEvent, EndMoveCallback);
	func(vtkCommand::InteractionEvent, MoveCallback);

}

void NRender::HandleWidgetActor::Start()
{
	__super::Start();
	if (m_RenderEngine)
	{
		m_Handle->SetInteractor(m_RenderEngine->GetInteractor());
		m_Handle->On();
	}
}

void NRender::HandleWidgetActor::Exit()
{
	__super::Exit();
	if (m_RenderEngine)
	{
		m_Handle->SetInteractor(m_RenderEngine->GetInteractor());
		m_Handle->Off();
	}
}

void NRender::HandleWidgetActor::SetActive(bool isShow)
{
	if (m_Handle)
	{
		m_Handle->SetEnabled(isShow);
	}
}

void NRender::HandleWidgetActor::SetEnable(bool isEnable)
{
	if (m_Handle)
	{
		m_Handle->SetEnabled(isEnable);
	}
}
