#pragma once

#include "Header.h"
#include "Types.h"
#include "vtkImageReslice.h"

class ResliceDelegate
{
public:
	ResliceDelegate();
	void Init(vtkImageData* data, MprType type);
	void Update();
	void UpdateSlice(int value);
	vtkMatrix4x4* GetResliceMatrix() { return ResliceMatrix; }
	vtkImageData* GetImageData();
	vtkImageReslice* GetImageReslice();
private:
	void SetMprMatrix(MprType ClipType);

	vtkImageData* m_Data;
	MprType m_Type;

	vtkSmartPointer<vtkMatrix4x4> ResliceMatrix;
	vtkSmartPointer<vtkImageReslice> Reslice;
};

