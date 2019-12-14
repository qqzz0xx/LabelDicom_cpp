#pragma once
#include "vtkCommand.h"
#include "vtkActor.h"

#include <functional>

namespace NRender 
{
	class NCommand : public vtkCommand
	{
	public:
		static NCommand* New();
		vtkTypeMacro(NCommand, vtkCommand);
		virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData) override;
	public:
		std::function<void(vtkObject *)> Callback = nullptr;
	};

}