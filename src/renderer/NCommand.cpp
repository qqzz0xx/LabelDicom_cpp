#include "NCommand.h"
#include "vtkObjectFactory.h"

using namespace NRender;

vtkStandardNewMacro(NCommand);

void NCommand::Execute(vtkObject *caller, unsigned long eventId, void *callData)
{
	if (Callback != nullptr)
	{
		Callback(caller);
	}
}
