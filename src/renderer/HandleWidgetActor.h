#pragma once
#include "Header.h"
#include "SceneNode.h"
#include "vtkHandleWidget.h"
#include "vtkPointHandleRepresentation2D.h"

namespace NRender
{
	class HandleWidgetActor : public SceneNode
	{
	public:
		HandleWidgetActor();

		virtual void Start() override;
		virtual void Exit() override;
		virtual void SetActive(bool isShow) override;
		virtual void SetEnable(bool isEnable) override;

		std::function<void(double* pos)> StartMoveCallback = nullptr;
		std::function<void(double* pos)> MoveCallback = nullptr;
		std::function<void(double* pos)> EndMoveCallback = nullptr;

	protected:
		vtkSmartPointer<vtkHandleWidget> m_Handle;
		vtkSmartPointer<vtkPointHandleRepresentation2D> m_HandleRep;
	};
}
