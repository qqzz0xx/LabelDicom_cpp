#include "NSliceInteractorStyle.h"
#include "vtkRendererCollection.h"
#include "vtkPropPicker.h"
#include <QEvent>
#include <QKeyEvent>

using namespace NRender;

vtkStandardNewMacro(NSliceInteractorStyle);

NSliceInteractorStyle::NSliceInteractorStyle()
{
	OnMouseMoveCallback = NULL;
	OnLeftButtonDownCallback = NULL;
	OnLeftButtonUpCallback = NULL;
	OnMiddleButtonDownCallback = NULL;
	OnMiddleButtonUpCallback = NULL;
	OnRightButtonDownCallback = NULL;
	OnRightButtonUpCallback = NULL;
	OnMouseWheelForwardCallback = NULL;
	OnMouseWheelBackwardCallback = NULL;
}


vtkProp3D * NSliceInteractorStyle::PickActor()
{
	// Pick from this location.
	vtkSmartPointer<vtkPropPicker>  picker =
		vtkSmartPointer<vtkPropPicker>::New();

	auto renderer = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
	auto xy = this->Interactor->GetEventPosition();
	picker->Pick(xy[0], xy[1], 0, renderer);
	double* pos = picker->GetPickPosition();
	auto pickActor = picker->GetProp3D();

	return pickActor;
}

void NSliceInteractorStyle::CheckAndRun(BoolCallback cb, VoidCallback baseCB)
{
	bool isBreak = false;
	if (cb)
	{
		isBreak = cb();
	}
	if (!isBreak)
	{
		baseCB();
	}
}

void NSliceInteractorStyle::OnMouseMove()
{
	CheckAndRun(OnMouseMoveCallback, [&] {vtkInteractorStyleImage::OnMouseMove(); });
}


void NSliceInteractorStyle::OnLeftButtonDown()
{
	LeftButtonStateChanged(true);
	CheckAndRun(OnLeftButtonDownCallback, [&] {vtkInteractorStyleImage::OnLeftButtonDown(); });
}

void NSliceInteractorStyle::OnLeftButtonUp()
{
	LeftButtonStateChanged(false);
	CheckAndRun(OnLeftButtonUpCallback, [&] {vtkInteractorStyleImage::OnLeftButtonUp(); });
}

void NSliceInteractorStyle::OnMiddleButtonDown()
{
	MiddleButtonStateChanged(true);
	CheckAndRun(OnMiddleButtonDownCallback, [&] {vtkInteractorStyleImage::OnMiddleButtonDown(); });
}

void NSliceInteractorStyle::OnMiddleButtonUp()
{
	MiddleButtonStateChanged(false);
	CheckAndRun(OnMiddleButtonUpCallback, [&] {vtkInteractorStyleImage::OnMiddleButtonUp(); });
}

void NSliceInteractorStyle::OnRightButtonDown()
{
	RightButtonStateChanged(true);
	CheckAndRun(OnRightButtonDownCallback, [&] {vtkInteractorStyleImage::OnRightButtonDown(); });
}

void NSliceInteractorStyle::OnRightButtonUp()
{
	RightButtonStateChanged(false);
	CheckAndRun(OnRightButtonUpCallback, [&] {vtkInteractorStyleImage::OnRightButtonUp(); });
}

void NSliceInteractorStyle::OnMouseWheelForward()
{
	CheckAndRun(OnMouseWheelForwardCallback, [&] {vtkInteractorStyleImage::OnMouseWheelForward(); });
}

void NSliceInteractorStyle::OnMouseWheelBackward()
{
	CheckAndRun(OnMouseWheelBackwardCallback, [&] {vtkInteractorStyleImage::OnMouseWheelBackward(); });
}

void NSliceInteractorStyle::OnKeyPress()
{
	CheckAndRun(OnKeyPressCallback, [&] {vtkInteractorStyleImage::OnKeyPress(); });
}

void NSliceInteractorStyle::OnKeyRelease()
{
	CheckAndRun(OnKeyReleaseCallback, [&] {vtkInteractorStyleImage::OnKeyRelease(); });
}

void NSliceInteractorStyle::LeftButtonStateChanged(bool v)
{
	LeftButtonPress = v;
}

void NSliceInteractorStyle::RightButtonStateChanged(bool v)
{
	RightButtonPress = v;
}

void NSliceInteractorStyle::MiddleButtonStateChanged(bool v)
{
	MiddleButtonPress = v;
}
