#include "RectangleActor.h"
#include "vtkRegularPolygonSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPlaneSource.h"
#include "LineActor.h"

using namespace NRender;

RectangleActor::RectangleActor()
{
	double p1[3] = { -0.5,0.5,0 };
	double p2[3] = { 0.5,0.5,0 };
	double p3[3] = { 0.5,-0.5,0 };
	double p4[3] = { -0.5,-0.5,0 };

	auto func = [&](auto t1, auto t2)
	{
		auto line = vtkSmartPointer<vtkActor>::New();
		auto lineSrc = vtkSmartPointer<vtkLineSource>::New();
		lineSrc->SetPoint1(t1);
		lineSrc->SetPoint2(t2);
		lineSrc->Update();

		auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputData(lineSrc->GetOutput());
		mapper->Update();
		line->SetMapper(mapper);
		line->GetProperty()->SetColor(1, 0, 0);
		return line;
	};

	auto actor = InitActor(vtkAssembly::New());

	actor->AddPart(func(p1, p2));
	actor->AddPart(func(p2, p3));
	actor->AddPart(func(p3, p4));
	actor->AddPart(func(p4, p1));

	m_Assembly = actor;

}


RectangleActor::~RectangleActor()
{
}


void NRender::RectangleActor::SetPoints(const glm::dvec3& p1, const glm::dvec3& p2)
{
	auto center = (p1 + p2) * 0.5;

	auto scaleX = glm::abs(p1.x - p2.x);
	auto scaleY = glm::abs(p1.y - p2.y);

	m_Assembly->SetScale(scaleX, scaleY, 1);
	SetPosition(glm::value_ptr(center));

	m_Points[0] = p1;
	m_Points[1] = p2;
}
