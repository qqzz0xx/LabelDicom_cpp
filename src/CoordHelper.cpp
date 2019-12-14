#include "CoordHelper.h"
#include "vtkCoordinate.h"
#include "AppMain.h"
#include "MainWindow.h"
#include "Types.h"

void CoordHelper::SceenToWorldPos(int* sceenPos, vtkRenderer* renderer, double* worldPos)
{
	auto coord = vtkCoordinate::New();
	coord->SetValue(sceenPos[0], sceenPos[1], 0);
	coord->SetCoordinateSystemToDisplay();
	auto pos = coord->GetComputedWorldValue(renderer);
	std::memcpy(worldPos, pos, sizeof(double) * 3);
	worldPos[2] = 0;
}

void CoordHelper::WorldToImageXY(double* point, NRender::SceneNode* imageActor, double* imageXY)
{
	auto mat = vtkSmartPointer<vtkMatrix4x4>::New();
	mat->DeepCopy(imageActor->GetWorldMat());
	mat->Invert();

	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(mat);
	auto p = transform->TransformDoublePoint(point);

	std::memcpy(imageXY, p, sizeof(double) * 3);
}

void CoordHelper::ImageXYToWorld(double* imageXY, NRender::SceneNode* imageActor, double* worldPos)
{
	auto mat = vtkSmartPointer<vtkMatrix4x4>::New();
	mat->DeepCopy(imageActor->GetWorldMat());

	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(mat);
	auto p = transform->TransformDoublePoint(imageXY);
	std::memcpy(worldPos, p, sizeof(double) * 3);
}

void CoordHelper::ImageXYToVoxPos(double* imageXY, vtkMatrix4x4* resliceMat, double* voxPos)
{
	auto mat = vtkSmartPointer<vtkMatrix4x4>::New();
	mat->DeepCopy(resliceMat);

	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(mat);
	auto p = transform->TransformDoublePoint(imageXY);

	std::memcpy(voxPos, p, sizeof(double) * 3);
}

void CoordHelper::VoxPosToImageXY(double* voxPos, vtkMatrix4x4* resliceMat, double* imageXY)
{
	auto mat = vtkSmartPointer<vtkMatrix4x4>::New();
	mat->DeepCopy(resliceMat);
	mat->Invert();

	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->SetMatrix(mat);
	auto p = transform->TransformDoublePoint(voxPos);

	std::memcpy(imageXY, p, sizeof(double) * 3);
}

void CoordHelper::WorldToVoxPos(double* worldPos, NRender::SceneNode* imageActor, vtkMatrix4x4* resliceMat, double* voxPos)
{
	double imageXY[3] = {};
	WorldToImageXY(worldPos, imageActor, imageXY);
	ImageXYToVoxPos(imageXY, resliceMat, voxPos);

}

void CoordHelper::VoxToWorldPos(double* voxPos, NRender::SceneNode* imageActor, vtkMatrix4x4* resliceMat, double* worldPos)
{
	double imageXY[3] = {};
	VoxPosToImageXY(voxPos, resliceMat, imageXY);
	ImageXYToWorld(imageXY, imageActor, worldPos);
}

void CoordHelper::VoxToWorldPos(double* voxPos, MprType type, double* worldPos)
{
	auto mpr = gMainWindow->GetMprComponent();
	auto sc = mpr->GetSliceComponent(type);
	auto view = sc->GetSliceView();

	CoordHelper::VoxToWorldPos(voxPos, view->GetSingleImageActor(), sc->GetReslice()->GetResliceMatrix(), worldPos);
}

void CoordHelper::WorldToVoxPos(double* worldPos, MprType type, double* voxPos)
{
	auto mpr = gMainWindow->GetMprComponent();
	auto sc = mpr->GetSliceComponent(type);
	auto view = sc->GetSliceView();

	CoordHelper::WorldToVoxPos(worldPos, view->GetSingleImageActor(), sc->GetReslice()->GetResliceMatrix(), voxPos);
}

size_t CoordHelper::GetVoxIndexByImageXY(double imgX, double imgY, MprType type, int sliderVal, int* dim)
{
	int x, y, z;
	if (type == MprType::None)
	{
		x = imgX;
		y = imgY;
		z = 0;
	}
	else if (type == MprType::Sagittal)
	{
		x = sliderVal;
		y = imgX;
		z = imgY;
	}
	else if (type == MprType::Coronal)
	{
		x = imgX;
		y = sliderVal;
		z = imgY;
	}
	else if (type == MprType::Axial)
	{
		x = imgX;
		y = imgY;
		z = sliderVal;
	}
	if ((x < 0 || x >= dim[0])
		|| (y < 0 || y >= dim[1])
		|| (z < 0 || z >= dim[2]))
	{
		return -1;
	}
	size_t index = x + y * dim[0] + dim[0] * dim[1] * z;
	return index;
}

