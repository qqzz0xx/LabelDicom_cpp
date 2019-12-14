#pragma once
#include "SceneNode.h"
#include "vtkImageMapToColors.h"
#include "vtkImageReslice.h"
#include "vtkColorTransferFunction.h"

namespace NRender
{
	class SingleImageActor : public SceneNode
	{
	public:
		SingleImageActor();

		void Init(vtkImageData* data);
		void ResetCamera();
		void SetImageData(vtkImageData* data);
		void Refresh();

		void SetLookupTable(vtkLookupTable* table);
		void SetWindowLevelWidth(int level, int width);
		void GetWindowLevelWidth(int& level, int& width);
		int* GetDim();
		int GetPixel(double x, double y);
		vtkImageData* GetImageData() { return m_ImageData; }

	private:
		vtkSmartPointer<vtkImageMapToColors> ImgToColors;
		vtkSmartPointer<vtkColorTransferFunction> ColorFunction;
		vtkSmartPointer<vtkImageActor> ImageActor;
		vtkImageData* m_ImageData;
		int m_Dim[3];
		int m_WindowWidth = 255;
		int m_WindowLevel = 128;
	};
}
