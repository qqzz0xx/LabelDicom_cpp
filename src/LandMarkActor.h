#pragma once
#include "Header.h"
#include "SceneNode.h"
#include "vtkTextActor.h"
#include "vtkTextActor3D.h"
#include "vtkRegularPolygonSource.h"
#include "vtkPlaneSource.h"

#include <QString>

namespace NRender
{
	class LandMarkActor : public SceneNode
	{
	public:
		LandMarkActor();

		void SetLabelColor(double* rgb);
		void SetLabelDesc(const QString& desc);

		vtkSmartPointer<vtkRegularPolygonSource> m_Source;
		vtkSmartPointer<vtkActor> m_Circle;
		vtkSmartPointer<vtkTextActor3D> m_TextActor;
		vtkSmartPointer<vtkPlaneSource> m_PlaneSrc;
		vtkSmartPointer<vtkActor> m_PlaneActor;

	};
}
