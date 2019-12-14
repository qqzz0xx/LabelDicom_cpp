#pragma once

#include "LineActor.h"
#include "vtkHandleWidget.h"

namespace NRender
{
	class RotateLineWidgetActor : public LineActor
	{
	public:
		RotateLineWidgetActor();
		virtual void Start() override;
		virtual void Exit() override;

	protected:
		struct stData
		{
			glm::dvec3 a, b, center;
		};

		stData m_Data;
		vtkSmartPointer<vtkHandleWidget> m_Handles[3];
	};
}