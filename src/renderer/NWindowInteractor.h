#pragma once

#include "vtkWin32RenderWindowInteractor.h"
#include "vtkObjectFactory.h"

namespace NRender
{
	class NWindowInteractor : public vtkWin32RenderWindowInteractor
	{
	public:
		static NWindowInteractor* New();
		vtkTypeMacro(NWindowInteractor, vtkWin32RenderWindowInteractor);

		NWindowInteractor();
		~NWindowInteractor();
	protected:
		virtual void StartEventLoop() override;
	public:
		void SetFrameBeginCallback(vtkCommand* cb) { m_pFrameBeginCallback = cb; }
		void SetFrameEndCallback(vtkCommand* cb) { m_pFrameEndCallback = cb; }
		void SetEnableLoopRender(bool loopRender) { m_enableLoopRender = loopRender; }
	protected:
		vtkCommand * m_pFrameBeginCallback = NULL;
		vtkCommand* m_pFrameEndCallback = NULL;
		bool m_enableLoopRender = true;

	};
}