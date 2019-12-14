#include "RenderEngine.h"
#include "RenderEngine.h"
#include "NCommand.h"
#include "AppMain.h"
#include "ModelManager.h"
#include "MprPositionModel.h"
#include "MainWindow.h"

#include "CoordHelper.h"

NRender::RenderEngine::RenderEngine()
{
	m_SceneManager = std::make_shared<SceneManager>(this);

	m_Renderer = vtkSmartPointer<vtkRenderer>::New();
	m_Renwin = vtkSmartPointer<vtkRenderWindow>::New();
	m_Interactor = vtkSmartPointer<NWindowInteractor>::New();
	m_InteractorStyle = vtkSmartPointer<NSliceInteractorStyle>::New();
	m_Renderer->GetActiveCamera()->SetParallelProjection(true);
	InitListener();
}

NRender::RenderEngine::~RenderEngine()
{
	m_SceneManager->Exit();
}

void NRender::RenderEngine::SetParallelProjection(bool v)
{
	m_Renderer->GetActiveCamera()->SetParallelProjection(v);
}

void NRender::RenderEngine::SetRenWinSize(int width, int height)
{
	if (m_Renwin)
	{
		m_Renwin->SetSize(width, height);
	}
}

void NRender::RenderEngine::SetRenWinParentId(void* ptr)
{
	if (m_Renwin)
	{
		m_Renwin->SetParentId(ptr);
	}
}

void NRender::RenderEngine::Start()
{

	m_Renderer->SetBackground(0, 0, 0);
	m_Renwin->AddRenderer(m_Renderer);
	m_Interactor->SetRenderWindow(m_Renwin);
	m_Interactor->SetInteractorStyle(m_InteractorStyle);
	m_Renwin->SetCurrentCursor(VTK_CURSOR_HAND);

	auto cmd = vtkSmartPointer<NCommand>::New();
	cmd->Callback = [&](auto ref) 
	{ 
		Update(m_Timer.GetElapsedSeconds()); 
	};
	m_Interactor->SetFrameBeginCallback(cmd);

	m_SceneManager->Start();
}

void NRender::RenderEngine::Update(float dt)
{
	m_SceneManager->Update(dt);
}

void NRender::RenderEngine::Exit()
{
	m_SceneManager->Exit();
}

void NRender::RenderEngine::Render()
{
	if (m_Renwin)
	{
		m_Renwin->Render();
	}
}

int* NRender::RenderEngine::GetEventPos()
{
	auto pos = m_Interactor->GetEventPosition();
	return pos;
}

double* NRender::RenderEngine::GetEventWorldPos()
{
	auto p = GetEventPos();

	CoordHelper::SceenToWorldPos(p, m_Renderer, m_EventWorldPos);
	m_EventWorldPos[2] = 0;

	return m_EventWorldPos;
}

void NRender::RenderEngine::OnMouseEvent(MouseEventType type)
{
	//auto model = gModelMgr->GetModel<MprPositionModel>();
	//auto p = GetEventPos();
	//model->m_SceenPos[0] = p[0];
	//model->m_SceenPos[1] = p[1];

	if (OnMouseEventCallback != nullptr)
	{
		OnMouseEventCallback(type);
	}

	m_SceneManager->OnMouseEvent(type);

	//gMainWindow->m_StatusLabel->setText(model->ToQtString());
}

void NRender::RenderEngine::InitListener()
{
	m_InteractorStyle->OnLeftButtonDownCallback = [&] {
		OnMouseEvent(MouseEventType_LeftDown);
		return true;
	};
	m_InteractorStyle->OnLeftButtonUpCallback = [&] {
		OnMouseEvent(MouseEventType_LeftUp);
		return true;
	};
	m_InteractorStyle->OnMiddleButtonDownCallback = [&] {
		OnMouseEvent(MouseEventType_MiddleDown);
		return true;
	};
	m_InteractorStyle->OnMiddleButtonUpCallback = [&] {
		OnMouseEvent(MouseEventType_MiddleDown);
		return true;
	};
	m_InteractorStyle->OnRightButtonDownCallback = [&] {
		OnMouseEvent(MouseEventType_RightDown);
		return true;
	};
	m_InteractorStyle->OnRightButtonUpCallback = [&] {
		OnMouseEvent(MouseEventType_RightUp);
		return true;
	};
	m_InteractorStyle->OnMouseWheelBackwardCallback = [&] {
		OnMouseEvent(MouseEventType_WheelBackward);
		return true;
	};
	m_InteractorStyle->OnMouseWheelForwardCallback = [&] {
		OnMouseEvent(MouseEventType_WheelForward);
		return true;
	};

	m_InteractorStyle->OnMouseMoveCallback = [&] {
		OnMouseEvent(MouseEventType_Move);
		return true;
	};

	m_InteractorStyle->OnKeyPressCallback = [&] {
		OnMouseEvent(MouseEventType_KeyPress);
		return false;
	};

	m_InteractorStyle->OnKeyReleaseCallback = [&] {
		OnMouseEvent(MouseEventType_KeyRelease);
		return false;
	};
}
