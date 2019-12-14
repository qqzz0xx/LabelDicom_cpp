#include "WinLevelWidthTools.h"
#include "FrameComponent.h"
#include "NWindowInteractor.h"
#include "RenderEngine.h"

WinLevelWidthTools::WinLevelWidthTools()
{
}


WinLevelWidthTools::~WinLevelWidthTools()
{
}

void WinLevelWidthTools::OnMouseEvent(MouseEventType type)
{
	/*
	if (type == MouseEventType_Move)
	{
		auto interactor = m_View->GetRenderEngine()->GetInteractor();
		auto istyle = m_View->GetRenderEngine()->GetInteractorStyle();
		if (istyle->LeftButtonPress)
		{
			auto lastPos = interactor->GetLastEventPosition();
			auto curPos = interactor->GetEventPosition();

			int level, width;

			auto actor = m_View->GetSingleImageActor();

			actor->GetWindowLevelWidth(level, width);
			level += curPos[0] - lastPos[0];
			width += curPos[1] - lastPos[1];

			actor->SetWindowLevelWidth(level, width);

			m_View->GetRenderEngine()->Render();

			if (OnWinLevelWidthCallback != nullptr)
			{
				OnWinLevelWidthCallback(m_View, level, width);
			}
		}
	}
	*/
}

