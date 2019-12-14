#pragma once
#include "Header.h"
#include "SceneNode.h"
#include "vtkRegularPolygonSource.h"
#include <QString>

namespace NRender
{
	class PointActor : public SceneNode
	{
	public:
		PointActor();
		void SetRadius(double r);
	protected:
		vtkSmartPointer<vtkRegularPolygonSource> m_Source;
		vtkSmartPointer<vtkActor> m_Circle;

	};
}
