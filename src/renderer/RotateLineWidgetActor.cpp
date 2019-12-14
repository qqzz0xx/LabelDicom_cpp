#include "RotateLineWidgetActor.h"
#include "RenderEngine.h"

NRender::RotateLineWidgetActor::RotateLineWidgetActor()
{
	for (size_t i = 0; i < 3; i++)
	{
		m_Handles[i] = vtkSmartPointer<vtkHandleWidget>::New();
	}
}

void NRender::RotateLineWidgetActor::Start()
{
	__super::Start();

	if (m_RenderEngine)
	{
		auto it = m_RenderEngine->GetInteractor();
		for (size_t i = 0; i < 3; i++)
		{
			m_Handles[i]->SetInteractor(it);
			m_Handles[i]->On();
		}
	}
}

void NRender::RotateLineWidgetActor::Exit()
{
	__super::Exit();

	if (m_RenderEngine)
	{
		auto it = m_RenderEngine->GetInteractor();
		for (size_t i = 0; i < 3; i++)
		{
			m_Handles[i]->SetInteractor(nullptr);
			m_Handles[i]->Off();
		}
	}
}
