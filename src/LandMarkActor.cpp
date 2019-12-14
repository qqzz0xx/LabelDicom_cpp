#include "LandMarkActor.h"
#include "vtkRegularPolygonSource.h"
#include "vtkTextActor3D.h"
#include "vtkTextProperty.h"
#include "vtkPlaneSource.h"
#include "Helper.h"

NRender::LandMarkActor::LandMarkActor()
{
	m_Source = vtkSmartPointer<vtkRegularPolygonSource>::New();

	//polygonSource->GeneratePolygonOff(); // Uncomment this line to generate only the outline of the circle
	m_Source->SetNumberOfSides(50);
	m_Source->SetRadius(2);
	m_Source->SetCenter(0, 0, 0);

	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(m_Source->GetOutputPort());

	m_Circle = vtkSmartPointer<vtkActor>::New();
	m_Circle->SetMapper(mapper);


	m_TextActor = vtkSmartPointer<vtkTextActor3D>::New();
	m_TextActor->SetInput("LandMark");
	m_TextActor->SetPosition(0, 10, 0);
	auto prop = m_TextActor->GetTextProperty();
	prop->SetFontFamilyToCourier();
	prop->SetBold(1);
	prop->SetFontSize(18);
	prop->SetColor(0, 0, 0);
	prop->SetOpacity(0.99);

	Helper::SetVtkFont(prop);

	m_Circle->SetPosition(0, 0, 0);
	m_Circle->GetProperty()->SetOpacity(0.8);

	m_PlaneSrc = vtkSmartPointer<vtkPlaneSource>::New();
	m_PlaneSrc->SetOrigin(0, 0, 0);
	m_PlaneSrc->SetPoint1(0, 23, 0);
	m_PlaneSrc->SetPoint2(100, 0, 0);
	m_PlaneSrc->Update();

	m_PlaneActor = vtkSmartPointer<vtkActor>::New();
	auto mapper1 = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper1->SetInputData(m_PlaneSrc->GetOutput());
	mapper1->Update();

	m_PlaneActor->SetMapper(mapper1);
	m_PlaneActor->GetProperty()->SetOpacity(0.5);
	m_PlaneActor->SetPosition(0, 10, 0);

	auto assmebly = InitActor(vtkAssembly::New());
	assmebly->AddPart(m_Circle);
	assmebly->AddPart(m_PlaneActor);
	assmebly->AddPart(m_TextActor);
}

void NRender::LandMarkActor::SetLabelColor(double* rgb)
{
	m_Circle->GetProperty()->SetColor(rgb);
	auto prop = m_PlaneActor->GetProperty();

	prop->SetColor(rgb);
}

void NRender::LandMarkActor::SetLabelDesc(const QString & desc)
{
	m_TextActor->SetInput(desc.toStdString().c_str());
	int len = m_TextActor->GetLength();
	m_PlaneSrc->SetOrigin(0, 0, 0);
	m_PlaneSrc->SetPoint1(0, 23, 0);
	m_PlaneSrc->SetPoint2(len, 0, 0);
	m_PlaneSrc->Update();
}
