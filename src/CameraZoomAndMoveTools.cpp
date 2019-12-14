#include "CameraZoomAndMoveTools.h"
#include "SliceView.h"
#include "CoordHelper.h"
#include "GlobalVal.h"

void CameraZoomAndMoveTools::OnMouseEvent(MouseEventType type)
{
	if (type == MouseEventType_Move)
	{
		auto istyle = m_View->GetRenderEngine()->GetInteractorStyle();
		auto iren = m_View->GetRenderEngine()->GetInteractor();
		auto lastPos = iren->GetLastEventPosition();
		auto curPos = iren->GetEventPosition();
		auto renderer = m_View->GetRenderEngine()->GetRenderer();
		auto camera = renderer->GetActiveCamera();

		if (istyle->RightButtonPress)
		{
			int dy = curPos[1] - lastPos[1];

			auto factor = dy * m_Factor;
			factor = std::pow(1.1, factor);


			if (camera->GetParallelProjection())
			{
				double scale = camera->GetParallelScale() / factor;
				camera->SetParallelScale(scale);

			 	double scaleFactor = scale / m_View->GetInitialCameraZoom();
				g_GlobalVal.CameraZoomFactor = scaleFactor;
				g_GlobalVal.PolyActorZoomFactor = glm::clamp(scaleFactor, 0.2, 4.0);

				if (ZoomChangedCallback != nullptr)
				{
					ZoomChangedCallback(scaleFactor);
				}
			}
			else
			{
				camera->Dolly(factor);
				renderer->ResetCameraClippingRange();
			}

			m_View->GetRenderEngine()->Render();
		}

		if (istyle->MiddleButtonPress)
		{
			double focalPos[3];
			camera->GetFocalPoint(focalPos);
			double cameraPos[3];
			camera->GetPosition(cameraPos);

			double curWorldPos[3];
			double lastWorldPos[3];

			CoordHelper::SceenToWorldPos(curPos, renderer, curWorldPos);
			CoordHelper::SceenToWorldPos(lastPos, renderer, lastWorldPos);

			double dx = curWorldPos[0] - lastWorldPos[0];
			double dy = curWorldPos[1] - lastWorldPos[1];

			focalPos[0] -= dx;
			focalPos[1] -= dy;
			cameraPos[0] -= dx;
			cameraPos[1] -= dy;

			camera->SetFocalPoint(focalPos);
			camera->SetPosition(cameraPos);
			m_View->GetRenderEngine()->Render();
		}
	}
}
