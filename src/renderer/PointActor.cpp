#include "PointActor.h"

NRender::PointActor::PointActor()
{
	m_Name = "PointActor";

	m_Source = vtkSmartPointer<vtkRegularPolygonSource>::New();

	//polygonSource->GeneratePolygonOff(); // Uncomment this line to generate only the outline of the circle
	m_Source->SetNumberOfSides(10);
	m_Source->SetRadius(3);
	m_Source->SetCenter(0, 0, 0);

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(m_Source->GetOutputPort());

	m_Circle = InitActor(vtkActor::New());
	m_Circle->SetMapper(mapper);

	m_Circle->GetProperty()->SetColor(1, 0, 0);
}

void NRender::PointActor::SetRadius(double r)
{
	m_Source->SetRadius(r);
	m_Source->Update();
}
