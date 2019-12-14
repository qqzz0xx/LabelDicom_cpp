#include "CubeActor.h"
#include "vtkCubeSource.h"
#include "vtkConeSource.h"

#include "MathHelper.h"

#include "glog/logging.h"

using namespace NRender;

CubeActor::CubeActor()
{
	m_Name = "CubeActor";
	auto m_Actor = InitActor(vtkActor::New());
	// Create a sphere
	vtkSmartPointer<vtkCubeSource> coneSource =
		vtkSmartPointer<vtkCubeSource>::New();
	coneSource->SetXLength(1);
	coneSource->SetYLength(1);
	coneSource->SetZLength(1);
	coneSource->Update();

	m_CubeSource = coneSource;

	// Create a mapper and actor
	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(coneSource->GetOutputPort());
	
	m_Actor->SetMapper(mapper);
}


CubeActor::~CubeActor()
{
}

void NRender::CubeActor::Update(float dt)
{
	__super::Update(dt);


}


void NRender::CubeActor::SetPosition(double x, double y, double z)
{
	m_Actor->SetPosition(x, y, z);
}

void NRender::CubeActor::SetColor(double r, double g, double b)
{
	auto actor = vtkActor::SafeDownCast(m_Actor);
	if (actor) actor->GetProperty()->SetColor(r, g, b);
}

void NRender::CubeActor::SetRGBA(double* rgba)
{
	auto actor = vtkActor::SafeDownCast(m_Actor);
	if (actor)
	{
		actor->GetProperty()->SetColor(rgba);
		actor->GetProperty()->SetOpacity(rgba[3] * 0.6);
	}
}

void NRender::CubeActor::SetXYZLength(double x, double y, double z)
{
	m_CubeSource->SetXLength(x);
	m_CubeSource->SetYLength(y);
	m_CubeSource->SetZLength(z);
	m_CubeSource->Update();
}
