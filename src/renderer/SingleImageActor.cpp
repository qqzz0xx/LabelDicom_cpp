#include "SingleImageActor.h"
#include "RenderEngine.h"
#include "vtkImageHistogram.h"

using namespace NRender;

NRender::SingleImageActor::SingleImageActor()
{
	m_Name = "SingleImageActor";

	ImgToColors = vtkSmartPointer<vtkImageMapToColors>::New();
	ColorFunction = vtkSmartPointer<vtkColorTransferFunction>::New();

	ImageActor = InitActor(vtkImageActor::New());
	ImageActor->SetInterpolate(false);
}

void NRender::SingleImageActor::Init(vtkImageData* data)
{
	m_ImageData = data;

	auto range = data->GetScalarRange();

	/*
	auto num = data->GetNumberOfPoints();

	auto p = (unsigned char*)data->GetScalarPointer();

	int presion = 100;

	std::map<int, int> hist;

	for (size_t i = 0; i < num; i++)
	{
		auto idx = p[i];
		auto it = hist.find(idx);
		if (it == hist.end())
		{
			hist[idx] = 1;
		}
		else
		{
			hist[idx] = hist[idx] + 1;
		}
	}

	for (auto&& [k, v] : hist)
	{
		if (v > presion)
		{
			range[0] = k;
			break;
		}
	}

	for (auto it = hist.rbegin(); it != hist.rend(); it++)
	{
		if (it->second > 5)
		{
			range[1] = it->first;
			break;
		}
	}
	*/

	m_WindowWidth = range[1] - range[0];
	m_WindowLevel = (range[1] + range[0]) * 0.5;

	double up = m_WindowLevel - m_WindowWidth * 0.5;
	double down = m_WindowLevel + m_WindowWidth * 0.5;


	ColorFunction->AddRGBPoint(0 , 0, 0, 0);
	ColorFunction->AddRGBPoint(255, 1, 1, 1);

	ImgToColors->SetLookupTable(ColorFunction);
	ImgToColors->SetInputData(data);
	ImgToColors->Update();

	ImageActor->SetInputData(ImgToColors->GetOutput());
	ImageActor->SetPosition(0, 0, 0);
}

void NRender::SingleImageActor::ResetCamera()
{
	if (m_RenderEngine && ImageActor)
	{
		m_RenderEngine->GetRenderer()->ResetCamera(ImageActor->GetBounds());
	}
}

void NRender::SingleImageActor::SetImageData(vtkImageData* data)
{
	m_ImageData = data;
	ImgToColors->SetInputData(data);
	ImageActor->SetInputData(ImgToColors->GetOutput());
}

void NRender::SingleImageActor::Refresh()
{
	ImgToColors->Modified();
	ImgToColors->Update();
	ImageActor->Update();
}

void NRender::SingleImageActor::SetLookupTable(vtkLookupTable * table)
{
	auto lookup = vtkSmartPointer<vtkLookupTable>::New();
	lookup->DeepCopy(table);

	ImgToColors->SetLookupTable(lookup);
}

void NRender::SingleImageActor::SetWindowLevelWidth(int level, int width)
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

void NRender::SingleImageActor::GetWindowLevelWidth(int & level, int & width)
{
	level = m_WindowLevel;
	width = m_WindowWidth;
}

int* NRender::SingleImageActor::GetDim()
{
	m_ImageData->GetDimensions(m_Dim);
	return m_Dim;
}

int NRender::SingleImageActor::GetPixel(double x, double y)
{
	auto dim = m_ImageData->GetDimensions();
	auto spc = m_ImageData->GetSpacing();
	x /= spc[0];
	y /= spc[1];
	x = std::round(x);
	y = std::round(y);

	if (x >= 0 && x < dim[0] && y >= 0 && y < dim[1])
	{
		auto ptr = (uint8*)m_ImageData->GetScalarPointer(x, y, 0);
		return *ptr;
	}

	return 0;
}

