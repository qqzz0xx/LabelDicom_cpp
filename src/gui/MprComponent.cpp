#include "MprComponent.h"
#include "CoordHelper.h"
#include "LocationTools.h"
#include "LandMarkTools.h"
#include "WinLevelWidthTools.h"
#include "CameraZoomAndMoveTools.h"
#include "PolygonTools.h"
#include "GlobalVal.h"
#include "BoundingBoxLabelModel.h"
#include "BoundingBoxTools.h"
#include "BoundingBox3DTools.h"
#include "BoundingBox3DLabelModel.h"

#include "MprPositionModel.h"
#include "AppMain.h"

MprComponent::MprComponent(QWidget* parent /* = Q_NULLPTR */, Qt::WindowFlags f /* = Qt::WindowFlags() */) :
	QWidget(parent),
	ui(new Ui_MprComponent)
{
	ui->setupUi(this);
	mpr[0] = ui->widget;
	mpr[1] = ui->widget_2;
	mpr[2] = ui->widget_3;

	for (auto& var : mpr)
	{
		var->OnMouseEventCallback = [&](auto slice, auto ev) {
			OnMouseEvent(slice, ev);
		};
		var->OnSliceValueChangedCallback = [&](auto slice, auto v) {
			OnSliderValueChanged(slice, v);
		};
	
	}

	InitTools();

	connect(gMainWindow->m_LabelEditorDialog,
		&LabelEditorDialog::LabelTypeChanged, this, &MprComponent::OnLabelTypeChanged);

	connect(ui->widget_4->ui->full, &QPushButton::clicked, [&](bool v) {
		OnSliceFullClicked(MprType::None, v);
	});
}

void MprComponent::Init(vtkImageData* data)
{
	m_bInit = false;

	auto func = [&](SliceComponent* sc, MprType ty)
	{
		sc->Init(data, ty);

		auto type = sc->GetMprType();
		connect(sc->ui->full, &QPushButton::clicked, [&, type](bool v) {
			OnSliceFullClicked(type, v);
		});
	};

	func(ui->widget, MprType::Sagittal);
	func(ui->widget_2, MprType::Coronal);
	func(ui->widget_3, MprType::Axial);

	ui->widget_4->InitPlaneWidget(data);
	ui->widget_4->InitLocation3D(data);

	m_bInit = true;
}

void MprComponent::OnMouseEvent(SliceComponent* slice, MouseEventType type)
{
	//auto ren = slice->GetRenderEngine();
	//auto model = gModelMgr->GetModel<MprPositionModel>();
	//auto resliceActor = slice->m_ResliceActor.get();
	//if (slice == nullptr || ren == nullptr || resliceActor == nullptr) return;

	//auto pos = ren->GetEventPos();
	//for (size_t i = 0; i < 2; i++)
	//{
	//	model->m_SceenPos[i] = pos[i];
	//}
	//CoordHelper::SceenToWorldPos(pos, ren->GetRenderer(), model->m_WorldPos);
	//CoordHelper::WorldToImageXY(model->m_WorldPos, resliceActor, model->m_ImageXY);
	//CoordHelper::ImageXYToVoxPos(model->m_ImageXY, resliceActor, model->m_VoxPos);
	//gMainWindow->m_StatusLabel->setText(model->ToQtString());

	/*if (m_bInit)
	{
		gAppMain->GetToolsTable()->ForEach([&](BaseMprTools* tools) {
			if (tools->GetEnable())
			{
				tools->OnMouseEvent(this, slice, type);
			}
		});
	}*/
}

void MprComponent::OnSliderValueChanged(SliceComponent* sc, int value)
{
	if (m_bInit)
	{
		auto imageActor = sc->GetSliceView()->GetSingleImageActor();
		auto resliceMat = sc->GetReslice()->GetResliceMatrix();
		auto wpos = sc->GetSliceView()->GetLocationActor()->GetActor()->GetPosition();

		double vpos[3];
		CoordHelper::WorldToVoxPos(wpos, imageActor, resliceMat, vpos);

		ui->widget_4->SetLocation3DPos(vpos);
		ui->widget_4->Render();
		//LOG(INFO) << "vpos:" << vpos[0] << " " << vpos[1] << " " << vpos[2];

		for (auto&& var : mpr)
		{
			if (var->GetMprType() != sc->GetMprType())
			{

				//var->SetIgnoreOnceSliceValueChanged();
				//var->SetSliderValueByVoxPos(vpos);

				imageActor = var->GetSliceView()->GetSingleImageActor();
				resliceMat = var->GetReslice()->GetResliceMatrix();
				double lwpos[3];
				CoordHelper::VoxToWorldPos(vpos, imageActor, resliceMat, lwpos);

				auto actor = var->GetSliceView()->GetLocationActor();
				actor->SetPosition(lwpos);

				//LOG(INFO) << "wpos:" << lwpos[0] << " " << lwpos[1] << " " << lwpos[2];

			}
		}

		//auto volume = ui->widget_4->GetVolumeActor();
		//if (volume != nullptr)
		//{
		//	volume->UpdateRelice(sc->GetMprType(), resliceMat);
		//	volume->ShowReslice(sc->GetMprType(), true);

		//	ui->widget_4->Render();
		//}
		Render();
	}
}

void MprComponent::RefreshMask()
{
	for (auto&& var : mpr)
	{
		var->GetMaskReslice()->Update();
		var->GetSliceView()->GetMaskImageActor()->Refresh();
	}
}

void MprComponent::Render()
{
	ui->widget->Render();
	ui->widget_2->Render();
	ui->widget_3->Render();
	ui->widget_4->Render();
}

void MprComponent::ForEach(std::function<void(SliceComponent*)> func)
{
	for (auto& var : mpr)
	{
		func(var);
	}
}

SliceComponent* MprComponent::GetSliceComponent(MprType type)
{
	switch (type)
	{
	case Sagittal:
		return mpr[0];
		break;
	case Coronal:
		return mpr[1];
		break;
	case Axial:
		return mpr[2];
		break;
	case None:
	default:
		break;
	}

	return nullptr;
}

int* MprComponent::GetCurSliceVal()
{
	m_SliceVals[0] = ui->widget->GetSliderValue();
	m_SliceVals[1] = ui->widget_2->GetSliderValue();
	m_SliceVals[2] = ui->widget_3->GetSliderValue();
	
	return m_SliceVals;
}

void MprComponent::InitTools()
{

	auto func = [&](SliceComponent* sc, auto t)
	{
		auto tools = sc->GetSliceView()->GetTools();
		t->Init(sc->GetSliceView());
		tools->Add(t);
	};

	for (auto&& slice : mpr)
	{
		auto locationTools = std::make_shared<LocationTools>();
		locationTools->LocationCallback = [&](auto view, auto imgXY) {
			OnLocationCallback(slice, imgXY);
		};

		auto zoomTools = std::make_shared<CameraZoomAndMoveTools>();
		zoomTools->ZoomChangedCallback = [&, slice](double scale) {
			OnZoomChanged(slice, scale);
		};

		func(slice, locationTools);
		func(slice, std::make_shared<WinLevelWidthTools>());
		func(slice, std::make_shared<LandMarkTools>());
		func(slice, std::make_shared<PolygonTools>());
		func(slice, std::make_shared<BoundingBoxTools>());
		func(slice, std::make_shared<BoundingBox3DTools>());
		func(slice, zoomTools);
	}

}

void MprComponent::OnLocationCallback(SliceComponent* sc, double* imgXY)
{
	double vpos[3];

	CoordHelper::ImageXYToVoxPos(imgXY, sc->GetReslice()->GetResliceMatrix(), vpos);
	LOG(INFO) << "location vpos:" << vpos[0] << " " << vpos[1] << " " << vpos[2];

	ui->widget_4->SetLocation3DPos(vpos);
	ui->widget_4->Render();

	for (auto&& var : mpr)
	{
		if (var->GetMprType() != sc->GetMprType())
		{
			var->SetIgnoreOnceSliceValueChanged();
			var->SetSliderValueByVoxPos(vpos);


			double wpos[3];
			CoordHelper::VoxToWorldPos(vpos, var->GetSliceView()->GetSingleImageActor(),
				var->GetReslice()->GetResliceMatrix(), wpos);
			auto actor = var->GetSliceView()->GetLocationActor();
			actor->SetPosition(wpos);

			LOG(INFO) << "location wpos:" << wpos[0] << " " << wpos[1] << " " << wpos[2];
		}
	}

	Render();
}

void MprComponent::OnLabelTypeChanged(ColorLabelItem* colorLabel)
{
	auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	auto lookup = model->GetLookupTable();

	for (int i = 0; i < 3; i++)
	{
		auto sc = mpr[i];
		auto reslice = sc->GetMaskReslice();
		reslice->GetResliceMatrix()->Modified();
		reslice->Update();
		auto actor = sc->GetSliceView()->GetMaskImageActor();
		if (actor)
		{
			actor->SetLookupTable(lookup);
			actor->Refresh();
			actor->SetActive(true);
		}
	}

	ui->widget_4->OnLabelTypeChanged(colorLabel);

	auto model1 = gModelMgr->GetModel<BoundingBoxLabelModel>();
	model1->OnColorLabelUpdate();
	auto model2 = gModelMgr->GetModel<BoundingBox3DLabelModel>();
	model2->OnColorLabelUpdate();

	Render();
}

void MprComponent::OnZoomChanged(SliceComponent* sc, double factor)
{
	for (auto&& var : mpr)
	{
		auto view = var->GetSliceView();
		auto re = view->GetRenderEngine();
		auto renderer = re->GetRenderer();

		auto& nodelist = re->GetSceneManager()->GetGlobleScene()->NodeList();
		for (auto&& var : nodelist)
		{
			var->OnCameraZoom(factor);
		}

		if (var->GetMprType() != sc->GetMprType())
		{
			auto v = view->GetInitialCameraZoom();
			auto camera = renderer->GetActiveCamera();
			camera->SetParallelScale(v * factor);
			re->Render();
		}
	}
}

void MprComponent::OnSliceFullClicked(MprType type, bool v)
{

	if (g_GlobalVal.IsInRandomSlice)
	{
		gMainWindow->ForEachRandomSlice([&](auto s) {
			s->setVisible(!v);
		});
	}
	else
	{
		for (auto&& var : mpr)
		{
			if (var->GetMprType() != type)
			{
				var->setVisible(!v);
			}
		}
	}

	if (type != MprType::None)
	{
		ui->widget_4->setVisible(!v);
	}
}
