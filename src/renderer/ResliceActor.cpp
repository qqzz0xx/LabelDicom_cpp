#include "ResliceActor.h"
#include "RenderEngine.h"

NRender::ResliceActor::ResliceActor()
{
	m_Name = "ResliceActor";

	Reslice = vtkSmartPointer< vtkImageReslice>::New();
	ImgToColors = vtkSmartPointer<vtkImageMapToColors>::New();
	ColorFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
	ResliceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	ImageShiftScale = vtkSmartPointer<vtkImageShiftScale>::New();

	ImageActor = InitActor(vtkImageActor::New());
}

NRender::ResliceActor::~ResliceActor()
{
}

void NRender::ResliceActor::Init(vtkImageData* data, MprType type)
{
	if (data == nullptr) return;
		
	m_Data = data;
	m_Type = type;

	auto spacing = data->GetSpacing();
	auto extent = data->GetExtent();
	auto dims = data->GetDimensions();

	double center[3] = { 0,0,0 };
	center[0] = spacing[0] * 0.5 * (extent[0] + extent[1]);
	center[1] = spacing[1] * 0.5 * (extent[2] + extent[3]);
	center[2] = spacing[2] * 0.5 * (extent[4] + extent[5]);
	ResliceMatrix->SetElement(0, 3, center[0]);
	ResliceMatrix->SetElement(1, 3, center[1]);
	ResliceMatrix->SetElement(2, 3, center[2]);
	SetMprMatrix(type);

	Reslice->SetInputData(data);
	Reslice->SetOutputDimensionality(2);
	Reslice->SetResliceAxes(ResliceMatrix);
	Reslice->SetInterpolationModeToLinear();
	Reslice->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
	Reslice->SetBackgroundColor(-2048, -2048, -2048, -2048);
	Reslice->Update();

	ImageShiftScale->SetInputData(Reslice->GetOutput());

	double up = m_WindowLevel - m_WindowLevel * 0.5;
	double down = m_WindowLevel + m_WindowWidth * 0.5;

	ColorFunction->AddRGBPoint(up, 0, 0, 0);
	ColorFunction->AddRGBPoint(down, 1, 1, 1);

	ImgToColors->SetLookupTable(ColorFunction);
	ImgToColors->SetInputData(Reslice->GetOutput());
	ImgToColors->Update();

	ImageActor->SetInputData(ImgToColors->GetOutput());
	ImageActor->SetPosition(0, 0, 0);

	ResetCamera();
	
}

void NRender::ResliceActor::Start()
{
	__super::Start();
	ResetCamera();
}

void NRender::ResliceActor::Update(float dt)
{

}

void NRender::ResliceActor::UpdateSlice(int sliceIdx)
{
	if (m_Data == nullptr) return;


	int* dim = m_Data->GetDimensions();

	double* spacing = m_Data->GetSpacing();


	switch (m_Type)
	{
	case MprType::Sagittal://Sagittal
		Reslice->SetResliceAxesOrigin(sliceIdx * spacing[0], dim[1] / 2.0 * spacing[1], dim[2] / 2.0 * spacing[2]);
		break;

	case MprType::Coronal://coronal
		Reslice->SetResliceAxesOrigin(dim[0] / 2.0 * spacing[0], sliceIdx * spacing[1], dim[2] / 2.0 * spacing[2]);
		break;
	case MprType::Axial://Axial
		Reslice->SetResliceAxesOrigin(dim[0] / 2.0 * spacing[0], dim[1] / 2.0 * spacing[1], sliceIdx * spacing[2]);
		break;
	case MprType::None://Volume
		break;
	default:
		break;
	}
	Reslice->SetResliceAxes(ResliceMatrix);
	Reslice->Update();
	ImageShiftScale->Update();
	ImgToColors->Update();
	ImageActor->Update();
}

void NRender::ResliceActor::ResetCamera()
{
	if (m_RenderEngine && ImageActor)
	{
		m_RenderEngine->GetRenderer()->ResetCamera(ImageActor->GetBounds());
	}
}

void NRender::ResliceActor::Refresh()
{
	Reslice->Update();
	ImageShiftScale->Update();
	ImgToColors->Update();
	ImageActor->Update();
}

void NRender::ResliceActor::SetWindowLevelWidth(int level, int width)
{
	m_WindowLevel = level;
	m_WindowWidth = width;

	ColorFunction->RemoveAllPoints();
	double up = level - width * 0.5;
	double down = level + width * 0.5;

	ColorFunction->AddRGBPoint(up, 0, 0, 0);
	ColorFunction->AddRGBPoint(down, 1, 1, 1);
	ColorFunction->Modified();

	ImgToColors->Update();
	ImageActor->Update();
}

void  NRender::ResliceActor::SetMprMatrix(MprType ClipType)
{
	switch (ClipType)
	{
	case MprType::Sagittal://Sagittal
		ResliceMatrix->SetElement(0, 0, 0);
		ResliceMatrix->SetElement(1, 0, -1/**/);
		ResliceMatrix->SetElement(0, 2, 1 /**/);
		ResliceMatrix->SetElement(2, 2, 0);
		ResliceMatrix->SetElement(1, 1, 0);
		ResliceMatrix->SetElement(2, 1, 1);
		break;
	case MprType::Coronal://Coronal
		ResliceMatrix->SetElement(1, 1, 0);
		ResliceMatrix->SetElement(2, 1, 1);
		ResliceMatrix->SetElement(1, 2, 1);
		ResliceMatrix->SetElement(2, 2, 0);

		break;
	case MprType::Axial://Axial
		ResliceMatrix->SetElement(1, 1, -1);
		break;
	case MprType::None://Axial
		ResliceMatrix->SetElement(1, 1, -1);
		break;
	default:
		break;
	}
}
