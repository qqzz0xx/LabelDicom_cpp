#pragma once
#include "Header.h"
#include "ResliceActor.h"

namespace CoordHelper
{
	void SceenToWorldPos(int* sceenPos, vtkRenderer* renderer, double* worldPos);

	void WorldToImageXY(double* point, NRender::SceneNode* imageActor, double* imageXY);

	void ImageXYToWorld(double* imageXY, NRender::SceneNode* imageActor, double* worldPos);

	void ImageXYToVoxPos(double* imageXY, vtkMatrix4x4* resliceMat, double* voxPos);

	void VoxPosToImageXY(double* voxPos, vtkMatrix4x4* resliceMat, double* imageXY);

	void WorldToVoxPos(double* worldPos, NRender::SceneNode* imageActor, vtkMatrix4x4* resliceMat, double* voxPos);
	
	void VoxToWorldPos(double* voxPos, NRender::SceneNode* imageActor, vtkMatrix4x4* resliceMat, double* worldPos);

	void VoxToWorldPos(double* voxPos, MprType type, double* worldPos);

	void WorldToVoxPos(double* worldPos, MprType type, double* voxPos);

	size_t GetVoxIndexByImageXY(double x, double y, MprType type, int sliderVal, int* dim);
};