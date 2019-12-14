#include "MeshActor.h"

#include "vtkImageConstantPad.h"
#include "vtkMarchingCubes.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkCommand.h"
#include "NCommand.h"
#include "vtkSphereSource.h"
#include "vtkStripper.h"

using namespace NRender;

MeshActor::MeshActor()
{
	m_Name = "MeshActor";
	InitActor(vtkActor::New());
}


MeshActor::~MeshActor()
{
}

void NRender::MeshActor::LoadFromImageData(vtkImageData * imagedata, int val)
{
	auto mesh = (vtkActor*)m_Actor;

	int* extent = imagedata->GetExtent();
	vtkSmartPointer<vtkImageConstantPad> padder = vtkSmartPointer<vtkImageConstantPad>::New();
	padder->SetInputData(imagedata);
	padder->SetConstant(0);
	padder->SetOutputWholeExtent(extent[0] - 2, extent[1] + 2,
		extent[2] - 2, extent[3] + 2,
		extent[4] - 2, extent[5] + 2);
	padder->Update();

	vtkSmartPointer<vtkMarchingCubes> mcubes = vtkSmartPointer<vtkMarchingCubes>::New();
	mcubes->SetInputData(padder->GetOutput());
	mcubes->SetValue(0, val);
	mcubes->ComputeScalarsOff();
	mcubes->ComputeGradientsOff();
	mcubes->ComputeNormalsOff();
	mcubes->Update();

	vtkSmartPointer<vtkStripper> stripper = vtkSmartPointer<vtkStripper>::New();
	stripper->SetInputData(mcubes->GetOutput());
	stripper->Update();

	//auto sincFilter = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
	//sincFilter->SetInputData(mcubes->GetOutput());
	//sincFilter->SetNumberOfIterations(100);
	//sincFilter->Update();

	LOG(INFO) << "vtkMarchingCubes\n";

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(stripper->GetOutput());
	mapper->Update();

	mesh->SetMapper(mapper);

	auto cmd = vtkSmartPointer<NCommand>::New();
	cmd->Callback = [&](auto obj) {

	};

	mesh->AddObserver(vtkCommand::LeftButtonPressEvent, cmd);
}

void NRender::MeshActor::Update(float dt)
{
	
}

