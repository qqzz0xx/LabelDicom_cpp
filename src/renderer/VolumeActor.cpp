#include "VolumeActor.h"
#include "ResliceActor.h"
#include "pugixml/pugixml.hpp"
#include "MathHelper.h"

#include <vtkBoxWidget.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkColorTransferFunction.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>
#include <vtkImageResample.h>
#include <vtkMetaImageReader.h>
#include <vtkPiecewiseFunction.h>
#include <vtkProperty.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkXMLImageDataReader.h>
#include <vtkNamedColors.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkImageActor.h>
#include <vtkImageMapToColors.h>
#include <vtkImageProperty.h>

using namespace NRender;


VolumeActor::VolumeActor()
{
	m_Name = "VolumeActor";
	m_Volume = InitActor(vtkVolume::New());
	m_VolumeData = vtkSmartPointer<vtkImageData>::New();
	//for (size_t i = 0; i < 4; i++)
	//{
	//	auto node = std::make_shared<ResliceActor>();
	//	m_ResliceNodeList.emplace_back(node);
	//	AddChild(node);
	//}
}


VolumeActor::~VolumeActor()
{
}

void NRender::VolumeActor::LoadFromImageData(vtkImageData* imagedata)
{
	m_VolumeData->DeepCopy(imagedata);

	vtkSmartPointer<vtkGPUVolumeRayCastMapper> volumeMapper =
		vtkSmartPointer<vtkGPUVolumeRayCastMapper>::New();
	volumeMapper->SetInputData(m_VolumeData);
	volumeMapper->Update();

	vtkSmartPointer<vtkVolumeProperty> volumeProperty =
		vtkSmartPointer<vtkVolumeProperty>::New();

	m_Volume->SetMapper(volumeMapper);
	m_Volume->SetProperty(volumeProperty);

	vtkSmartPointer<vtkColorTransferFunction> colorFun = vtkSmartPointer<vtkColorTransferFunction>::New();
	vtkSmartPointer<vtkPiecewiseFunction> opacityFun = vtkSmartPointer<vtkPiecewiseFunction>::New();

	volumeProperty->SetColor(colorFun);
	volumeProperty->SetScalarOpacity(opacityFun);

	volumeMapper->SetBlendModeToComposite();
	volumeProperty->ShadeOn();

	opacityFun->AddPoint(0.0, 0.0);
	//opacityFun->AddPoint(80.0, 1.0);
	//opacityFun->AddPoint(80.1, 1.0);
	opacityFun->AddPoint(255.0, 1.0);
	volumeProperty->SetScalarOpacity(opacityFun); // composite first.

	vtkSmartPointer<vtkColorTransferFunction> color =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	colorFun->AddRGBPoint(0.0, 0.0, 0.0, 1.0);
	colorFun->AddRGBPoint(40.0, 1.0, 0.0, 0.0);
	colorFun->AddRGBPoint(255.0, 1.0, 1.0, 1.0);
	volumeProperty->SetColor(color);

	m_Volume->SetUserMatrix(m_WorldMat);

	//InitReslice(m_VolumeData);

	ResetCameraToMe();
}

void NRender::VolumeActor::SetColorTable(std::string fileName)
{
	vtkVolumeProperty* volumeProperty = m_Volume->GetProperty();

	volumeProperty->GetScalarOpacity()->RemoveAllPoints();
	volumeProperty->GetRGBTransferFunction()->RemoveAllPoints();

	pugi::xml_document doc;
	doc.load_file(fileName.c_str());
	auto root = doc.first_child();
	m_CurColorTableName = root.child("ColorTableName").value();

	auto OpacityTransferFunction = root.child("OpacityTransferFunction");
	for (auto& var : OpacityTransferFunction)
	{
		double _val[4];
		_val[0] = var.attribute("gray_value").as_double();
		_val[1] = var.attribute("opacity").as_double();
		_val[2] = var.attribute("midpoint").as_double();
		_val[3] = var.attribute("sharpness").as_double();
		volumeProperty->GetScalarOpacity()->AddPoint(_val[0], _val[1], _val[2], _val[3]);
	}

	auto ColorTransferFunction = root.child("ColorTransferFunction");
	for (auto& var : ColorTransferFunction)
	{
		double _val[6];
		_val[0] = var.attribute("gray_value").as_double();
		_val[1] = var.attribute("red").as_double();
		_val[2] = var.attribute("green").as_double();
		_val[3] = var.attribute("blue").as_double();
		_val[4] = var.attribute("midpoint").as_double();
		_val[5] = var.attribute("sharpness").as_double();

		volumeProperty->GetRGBTransferFunction()->AddRGBPoint(_val[0], _val[1], _val[2], _val[3], _val[4], _val[5]);
	}

	auto Light = root.child("Light");
	for (auto& var : Light)
	{
		double _val[4];
		_val[0] = var.attribute("Diffuse").as_double();
		_val[1] = var.attribute("Ambient").as_double();
		_val[2] = var.attribute("Specular").as_double();
		_val[3] = var.attribute("SpecularPower").as_double();

		volumeProperty->SetDiffuse(_val[0]);
		volumeProperty->SetAmbient(_val[1]);
		volumeProperty->SetSpecular(_val[2]);
		volumeProperty->SetSpecularPower(_val[3]);
	}
	{
		auto Window = root.child("Window");
		m_WindowLevel = Window.first_child().attribute("WindowLevel").as_double();
		m_WindowWidth = Window.first_child().attribute("WindowWidth").as_double();
	}
}

void NRender::VolumeActor::MoveColorTable(double delta)
{
	vtkVolumeProperty* volumeProperty = m_Volume->GetProperty();

	// Create our transfer function
	vtkSmartPointer<vtkColorTransferFunction> colorFun = vtkSmartPointer<vtkColorTransferFunction>::New();
	//vtkSmartPointer<vtkPiecewiseFunction> opacityFun = vtkSmartPointer<vtkPiecewiseFunction>::New();
	colorFun->DeepCopy(volumeProperty->GetRGBTransferFunction());
	//opacityFun->DeepCopy(volumeProperty->GetScalarOpacity());

	//volumeProperty->GetScalarOpacity()->RemoveAllPoints();
	//volumeProperty->GetRGBTransferFunction()->RemoveAllPoints();
	auto opacityFun = volumeProperty->GetScalarOpacity();
	opacityFun->RemoveAllPoints();

	opacityFun->AddPoint(0.0 + delta * 2, 0.0);
	opacityFun->AddPoint(255.0 + delta * 2, 1.0);
	opacityFun->Modified();

	//for (int i = 0; i < opacityFun->GetSize(); ++i)
	//{
	//	double val[4];
	//	opacityFun->GetNodeValue(i, val);
	//	volumeProperty->GetScalarOpacity()->AddPoint(val[0] + delta, val[1], val[2], val[3]);
	//}
	//for (int i = 0; i < colorFun->GetSize(); ++i)
	//{
	//	double val[6];
	//	colorFun->GetNodeValue(i, val);
	//	volumeProperty->GetRGBTransferFunction()->AddRGBPoint(val[0] + delta,
	//		val[1],
	//		val[2],
	//		val[3],
	//		val[4],
	//		val[5]);
	//}
}

vtkImageData* NRender::VolumeActor::GetImageData()
{
	if (m_Volume == nullptr) return nullptr;

	auto mapper = m_Volume->GetMapper();
	auto rayMapper = vtkGPUVolumeRayCastMapper::SafeDownCast(mapper);

	return rayMapper->GetInput();
}

vtkGPUVolumeRayCastMapper* NRender::VolumeActor::GetMapper()
{
	auto mapper = m_Volume->GetMapper();
	auto rayMapper = vtkGPUVolumeRayCastMapper::SafeDownCast(mapper);
	return rayMapper;
}

void NRender::VolumeActor::Update(float dt)
{
	auto m_Volume = vtkVolume::SafeDownCast(m_Actor);
	for (auto& var : m_ChildNodes)
	{
		var->Update(dt);
	}
	//if (  )
	//{
	//	MoveColorTable(inputMgr->deltaMousePosition[0]);
	//}

	CheckPickAndMove(m_Volume);
}

void NRender::VolumeActor::InitImageData(vtkImageData* data)
{
	m_VolumeData->DeepCopy(data);
	InitReslice(m_VolumeData);
}

void NRender::VolumeActor::UpdateRelice(MprType type, int sliceIdx)
{
	if (m_ResliceNodeList.empty()) return;
	auto node = m_ResliceNodeList[type];
	if (node->GetActive())
	{
		node->UpdateSlice(sliceIdx);
		node->SetLocalMat(node->GetResliceMatrix());
	}
}

void NRender::VolumeActor::UpdateRelice(MprType type, vtkMatrix4x4* mat)
{
	if (m_ResliceNodeList.empty()) return;
	auto node = m_ResliceNodeList[type];
	if (node->GetActive())
	{
		auto resMat = node->GetResliceMatrix();
		resMat->DeepCopy(mat);
		resMat->Modified();

		node->Refresh();
		node->SetLocalMat(resMat);
	}
}

void NRender::VolumeActor::UpdateResliceLevelWindow(MprType type, double level, double window)
{

	for (auto& var : m_ResliceNodeList)
	{

		var->SetWindowLevelWidth(level, window);
		/*	auto prop = reslice->ImageActor->GetProperty();
			prop->SetColorLevel(level);
			prop->SetColorWindow(window);*/
	}
}

void NRender::VolumeActor::ShowReslice(MprType type, bool isShow)
{
	if (m_ResliceNodeList.empty()) return;
	auto node = m_ResliceNodeList[type];
	node->SetActive(isShow);
}

void NRender::VolumeActor::SetReliceAlpha(double alpha)
{
	for (auto& var : m_ResliceNodeList)
	{
		auto actor = vtkImageActor::SafeDownCast(var->GetActor());

		actor->GetProperty()->SetOpacity(alpha);
	}
}

void NRender::VolumeActor::ShowVolume(bool v)
{
	if (m_Volume)
	{
		m_Volume->SetVisibility(v);
	}
}

void NRender::VolumeActor::Start()
{
	__super::Start();
	ResetCameraToMe();
}

void NRender::VolumeActor::InitReslice(vtkImageData* data)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (i >= m_ResliceNodeList.size())
		{
			auto node = std::make_shared<ResliceActor>();
			AddChild(node);
			m_ResliceNodeList.emplace_back(node);

			node->SetActive(true);

		}

		auto resliceActor = m_ResliceNodeList[i];
		resliceActor->Init(data, (MprType)i);
		resliceActor->SetWindowLevelWidth(100, 255);
		resliceActor->SetLocalMat(resliceActor->GetResliceMatrix());
	}
}



