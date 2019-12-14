#pragma once
#include "Header.h"
#include "SceneNode.h"

#include "vtkBoxWidget2.h"
#include "vtkBoxWidget.h"
#include "vtkWidgetRepresentation.h"

namespace NRender
{
	class BoxWidgetActor : public SceneNode
	{
	public:
		BoxWidgetActor();

		vtkBoxRepresentation* GetRepresentation();
		void SetRGBA(double* rgba);
		void SetBounds(double* bds);
		void SetTransform(vtkTransform* trans);

		std::function<void()> StartMoveCallback = nullptr;
		std::function<void()> MoveCallback = nullptr;
		std::function<void()> EndMoveCallback = nullptr;

		void SetSelected(bool v);

		vtkSmartPointer<vtkBoxWidget2> m_Widget;

		virtual void Start() override;

		virtual void Exit() override;

		virtual void SetActive(bool isShow) override;

		virtual void SetEnable(bool isEnable) override;

		vtkMatrix4x4* GetMatrix() { return m_Matrix; };
		double* GetCenter();

	private:
		void OnInteractor(int ev);

		vtkSmartPointer<vtkMatrix4x4> m_Matrix;
		double m_Center[3];
	};
}