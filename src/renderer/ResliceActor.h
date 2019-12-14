#pragma once
#include "Header.h"
#include "Types.h"
#include "SceneNode.h"

#include "vtkColorTransferFunction.h"
#include "vtkImageMapToColors.h"
#include "vtkImageReslice.h"
#include "vtkImageShiftScale.h"

namespace NRender
{
	class ResliceActor : public SceneNode
	{
	public:
		ResliceActor();
		~ResliceActor();

		void Init(vtkImageData* data, MprType type);
		virtual void Start() override;
		virtual void Update(float dt) override;
		void UpdateSlice(int value);
		void ResetCamera();
		void Refresh();
		void SetWindowLevelWidth(int level, int width);
		vtkMatrix4x4* GetResliceMatrix() { return ResliceMatrix; }
	private:
		void SetMprMatrix(MprType ClipType);
	private:
		vtkImageData* m_Data;
		MprType m_Type;

		vtkSmartPointer< vtkImageReslice> Reslice;
		vtkSmartPointer<vtkImageMapToColors>	ImgToColors;
		vtkSmartPointer<vtkColorTransferFunction> ColorFunction;
		vtkSmartPointer< vtkImageActor> ImageActor;
		vtkSmartPointer<vtkMatrix4x4> ResliceMatrix;
		vtkSmartPointer<vtkImageShiftScale> ImageShiftScale;

		int m_WindowWidth = 255;
		int m_WindowLevel = 128;
	};
}
