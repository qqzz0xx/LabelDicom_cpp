#include "ResliceDelegate.h"

ResliceDelegate::ResliceDelegate()
{
	ResliceMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	Reslice = vtkSmartPointer< vtkImageReslice>::New();
}

void ResliceDelegate::Init(vtkImageData* data, MprType type)
{
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
	Reslice->SetInterpolationModeToNearestNeighbor();

	switch (type)
	{
	case Sagittal:
		Reslice->SetOutputSpacing(spacing[1], spacing[2], 1);
		break;
	case Coronal:
		Reslice->SetOutputSpacing(spacing[0], spacing[2], 1);
		break;
	case Axial:
		Reslice->SetOutputSpacing(spacing[0], spacing[1], 1);
		break;
	default:
		Reslice->SetOutputSpacing(spacing[0], spacing[1], spacing[2]);
		break;
	}
	
	Reslice->SetBackgroundColor(-2048, -2048, -2048, -2048);
	Reslice->Update();
}

void ResliceDelegate::UpdateSlice(int sliceIdx)
{
	if (m_Data == nullptr) return;

	int* dim = m_Data->GetDimensions();
	double* spacing = m_Data->GetSpacing();

	int center[3];
	for (size_t i = 0; i < 3; i++)
	{
		center[i] = (int)((dim[i] - 1) / 2.0 * spacing[i]);
	}

	switch (m_Type)
	{
	case MprType::Sagittal://Sagittal
		Reslice->SetResliceAxesOrigin(sliceIdx * spacing[0], center[1], center[2]);
		break;
	case MprType::Coronal://coronal
		Reslice->SetResliceAxesOrigin(center[0], sliceIdx * spacing[1], center[2]);
		break;
	case MprType::Axial://Axial
		Reslice->SetResliceAxesOrigin(center[0], center[1], sliceIdx * spacing[2]);
		break;
	case MprType::None://Volume
		break;
	default:
		break;
	}

	Reslice->SetResliceAxes(ResliceMatrix);
	Reslice->Update();
}

vtkImageData* ResliceDelegate::GetImageData()
{
	return Reslice->GetOutput();
}

vtkImageReslice* ResliceDelegate::GetImageReslice()
{
	return Reslice;
}

void ResliceDelegate::Update()
{
	ResliceMatrix->Modified();
	Reslice->Update();
}

void ResliceDelegate::SetMprMatrix(MprType ClipType)
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
