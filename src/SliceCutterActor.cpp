#include "SliceCutterActor.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkPerspectiveTransform.h"
#include "vtkImageReslice.h"

NRender::SliceCutterActor::SliceCutterActor()
{
	m_Name = "SliceCutterActor";
	auto actor = InitActor(vtkActor::New());

	auto prop = vtkSmartPointer<vtkProperty>::New();
	prop->SetColor(1, 1, 0);
	prop->SetOpacity(0.6);
	prop->SetLighting(false);
	actor->SetProperty(prop);
	//actor->SetBackfaceProperty(prop);
	//SetAlpha(0.6);
}

NRender::SliceCutterActor::~SliceCutterActor()
{
	m_Src->Delete();
}

void NRender::SliceCutterActor::SetSource(vtkPolyDataAlgorithm* src)
{
	m_Src = src;
}

void NRender::SliceCutterActor::Refresh()
{
	auto userMat = vtkSmartPointer<vtkMatrix4x4>::New();
	userMat->DeepCopy(m_VoxMat);

	auto cosines = m_Reslice->GetResliceAxesDirectionCosines();
	auto resliceOrig = m_Reslice->GetResliceAxesOrigin();

	auto volumeToResliceCoord = vtkSmartPointer<vtkPerspectiveTransform>::New();
	volumeToResliceCoord->Identity();
	volumeToResliceCoord->SetupCamera(resliceOrig[0], resliceOrig[1], resliceOrig[2],
		resliceOrig[0] + cosines[6], resliceOrig[1] + cosines[7], resliceOrig[2] + cosines[8],
		cosines[3], cosines[4], cosines[5]);
	volumeToResliceCoord->Update();

	auto voxToResliceMat = volumeToResliceCoord->GetMatrix();

	vtkMatrix4x4::Multiply4x4(voxToResliceMat, userMat, userMat);

	auto voxToResliceMatInv = vtkSmartPointer<vtkMatrix4x4>::New();
	voxToResliceMatInv->DeepCopy(userMat);
	voxToResliceMatInv->Invert();

	auto trans = vtkSmartPointer<vtkTransform>::New();
	trans->SetMatrix(voxToResliceMatInv);
	trans->Update();

	double pNor[3] = { 0,0,1 };
	double pOri[3] = { 0 };

	auto plane = vtkSmartPointer<vtkPlane>::New();
	double tOri[3];
	double tNor[3];
	std::memcpy(tOri, trans->TransformDoublePoint(pOri), sizeof(tOri));
	plane->SetOrigin(tOri);
	std::memcpy(tNor, trans->TransformDoubleNormal(pNor), sizeof(tNor));
	plane->SetNormal(tNor);

	auto cutter = vtkSmartPointer<vtkCutter>::New();
	cutter->SetCutFunction(plane);
	cutter->SetInputConnection(m_Src->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> mapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(cutter->GetOutputPort());
	vtkActor::SafeDownCast(m_Actor)->SetMapper(mapper);

	auto resliceMatInv = vtkSmartPointer<vtkMatrix4x4>::New();
	auto mat = m_Reslice->GetResliceAxes();
	resliceMatInv->DeepCopy(mat);
	resliceMatInv->Invert();
	vtkMatrix4x4::Multiply4x4(resliceMatInv, m_VoxMat, m_WorldMat);

	//qDebug() << "Reslice:" << mat->Element[0][3] << "," << mat->Element[1][3] << "," << mat->Element[2][3];
	//qDebug() << "m_VoxMat:" << m_VoxMat->Element[0][3] << "," << m_VoxMat->Element[1][3] << "," << m_VoxMat->Element[2][3];
	//qDebug() << "m_WorldMat:" << m_WorldMat->Element[0][3] << "," << m_WorldMat->Element[1][3] << "," << m_WorldMat->Element[2][3];
	
	m_WorldMat->Element[2][3] += 1;
	m_WorldMat->Modified();
	m_Actor->Modified();
}
