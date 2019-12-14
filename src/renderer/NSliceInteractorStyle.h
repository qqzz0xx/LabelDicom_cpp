#pragma once
#include "Header.h"
#include "Types.h"
#include "vtkInteractorStyleImage.h"

namespace NRender
{
	class NSliceInteractorStyle : public vtkInteractorStyleImage
	{
	public:
		static NSliceInteractorStyle* New();
		NSliceInteractorStyle();
		vtkTypeMacro(NSliceInteractorStyle, vtkInteractorStyleImage);

		vtkProp3D* PickActor();
		void CheckAndRun(BoolCallback cb, VoidCallback baseCB);


		virtual void OnMouseMove() override;

		virtual void OnLeftButtonDown() override;

		virtual void OnLeftButtonUp() override;

		virtual void OnMiddleButtonDown() override;

		virtual void OnMiddleButtonUp() override;

		virtual void OnRightButtonDown() override;

		virtual void OnRightButtonUp() override;

		virtual void OnMouseWheelForward() override;

		virtual void OnMouseWheelBackward() override;

		virtual void OnKeyPress() override;

		virtual void OnKeyRelease() override;

	public:
		BoolCallback OnMouseMoveCallback;
		BoolCallback OnLeftButtonDownCallback;
		BoolCallback OnLeftButtonUpCallback;
		BoolCallback OnMiddleButtonDownCallback;
		BoolCallback OnMiddleButtonUpCallback;
		BoolCallback OnRightButtonDownCallback;
		BoolCallback OnRightButtonUpCallback;
		BoolCallback OnMouseWheelForwardCallback;
		BoolCallback OnMouseWheelBackwardCallback;

		BoolCallback OnKeyPressCallback;
		BoolCallback OnKeyReleaseCallback;

		bool LeftButtonPress = false;
		bool RightButtonPress = false;
		bool MiddleButtonPress = false;

	private:

		void LeftButtonStateChanged(bool v);
		void RightButtonStateChanged(bool v);
		void MiddleButtonStateChanged(bool v);
	};

}
