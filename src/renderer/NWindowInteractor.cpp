#include "NWindowInteractor.h"
#include "vtkCommand.h"

using namespace NRender;

vtkStandardNewMacro(NWindowInteractor);

NWindowInteractor::NWindowInteractor()
{
}


NWindowInteractor::~NWindowInteractor()
{
}

void NWindowInteractor::StartEventLoop()
{
	// No need to do anything if this is a 'mapped' interactor
	if (!this->Enabled || !this->InstallMessageProc)
	{
		return;
	}

	this->StartedMessageLoop = 1;

	MSG msg;
	bool isRunning = true;
	while (isRunning)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))       // ����Ϣ�ڵȴ���?  
		{
			if (msg.message == WM_QUIT)       // �յ��˳���Ϣ?  
			{
				isRunning = false;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (m_enableLoopRender && m_pFrameBeginCallback != NULL) m_pFrameBeginCallback->Execute(NULL, 0, NULL);

			if (m_enableLoopRender && m_pFrameEndCallback != NULL) m_pFrameEndCallback->Execute(NULL, 0, NULL);
		}
	}
}
