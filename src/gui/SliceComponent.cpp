#include "SliceComponent.h"
#include "vtkImageCanvasSource2D.h"
#include "NCommand.h"
#include "AppMain.h"
#include "CoordHelper.h"
#include "MprPositionModel.h"
#include "LandMarkTools.h"
#include "LandMarkListModel.h"
#include "LoadMgr.h"
#include "GlobalVal.h"

#include <QElapsedTimer>

using namespace NRender;

SliceComponent::SliceComponent(QWidget * parent, Qt::WindowFlags f) :
	QWidget(parent, f),
	ui(new Ui_SliceComponent)
{
	ui->setupUi(this);
	m_View = ui->widget;

	m_View->OnMouseEventCallback = [&](SliceView* view, MouseEventType ev)
	{
		OnMouseEvent(ev);
	};

	connect(ui->horizontalSlider, &QSlider::rangeChanged, this, [&](int min, int max) {
		ui->spinBox->setRange(min, max);
	});


	//m_RenderEngine = std::make_shared<RenderEngine>();
	//m_RenderEngine->OnMouseEventCallback = [&](auto ev) {OnMouseEvent(ev); };

	//int width = ui->label->size().width();
	//int height = ui->label->size().height();
	//m_RenderEngine->SetRenWinSize(width, height);
	//m_RenderEngine->SetRenWinParentId((void*)ui->label->winId());

	//m_RenderEngine->Start();
	//m_RenderEngine->Render();
}

SliceComponent::~SliceComponent()
{
	//m_LocationActor.reset();
	//m_ResliceActor.reset();
	//m_RenderEngine.reset();
}

void SliceComponent::Init(vtkImageData* data, MprType type)
{
	if (data == nullptr) return;
	m_Type = type;
	m_ImageData = data;

	m_Reslice.Init(data, type);
	m_View->Init(m_Reslice.GetImageData(), type);

	m_MaskReslice.Init(gLoadMgr->GetCurMaskData(), type);

	auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	auto lookup = model->GetLookupTable();
	auto imageActor = m_View->GetMaskImageActor();
	imageActor->Init(m_MaskReslice.GetImageData());
	imageActor->SetLookupTable(lookup);
	imageActor->Refresh();
	imageActor->SetActive(true);

	//if (m_ResliceActor == nullptr)
	//{
	//	m_ResliceActor = std::make_shared<ResliceActor>();
	//	m_ResliceActor->Init(gLoadMgr->GetImageData(), type);
	//	m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(m_ResliceActor);
	//}
	//else
	//{
	//	m_ResliceActor->Init(gLoadMgr->GetImageData(), type);
	//}

	//if (m_LocationActor == nullptr)
	//{
	//	m_LocationActor = std::make_shared<LocationActor>();
	//	m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(m_LocationActor);
	//	m_LocationActor->SetColorOfType(m_Type);
	//}
	//

	InitSliderRange(data->GetDimensions());

	Render();
}

void SliceComponent::InitSliderRange(int* dim)
{
	int max = 0;
	switch (m_Type)
	{
	case MprType::None:
		max = dim[2] - 1;
		break;
	case MprType::Axial:
		max = dim[2] - 1;
		break;
	case MprType::Sagittal:
		max = dim[0] - 1;
		break;
	case MprType::Coronal:
		max = dim[1] - 1;
		break;
	default:
		break;
	}

	ui->horizontalSlider->setMinimum(0);
	ui->horizontalSlider->setMaximum(max);
	ui->horizontalSlider->setSliderPosition(max * 0.5);

	ui->label_2->setText(QString::number(max));
}

void SliceComponent::RefreshMask()
{
	int v = ui->horizontalSlider->value();
	//ui->horizontalSlider->setValue(v);
	m_MaskReslice.UpdateSlice(v);
	auto imageActor = m_View->GetMaskImageActor();
	imageActor->SetImageData(m_MaskReslice.GetImageData());
	imageActor->Refresh();

	m_Reslice.UpdateSlice(v);
	m_View->Render(m_Reslice.GetImageData());
}

void SliceComponent::Render()
{
	/*if (m_RenderEngine)
	{
		m_RenderEngine->Render();
	}*/

	m_View->Render();
}

void SliceComponent::SetSliderValueByVoxPos(double* voxPos)
{
	int idx = -1;
	auto spacing = m_ImageData->GetSpacing();

	if (m_Type == Sagittal)
	{
		idx = 0;
	}
	else if (m_Type == Coronal)
	{
		idx = 1;
	}
	else if (m_Type == Axial)
	{
		idx = 2;
	}

	double v = voxPos[idx] / spacing[idx];
	int iv = std::round(v);
	ui->horizontalSlider->setSliderPosition(iv);
}

void SliceComponent::SetIgnoreOnceSliceValueChanged()
{
	m_IgnoreOnceSliceValueChanged = true;
}

void SliceComponent::on_horizontalSlider_valueChanged(int v)
{
	//if (m_ResliceActor)
	//{
	//	m_ResliceActor->UpdateSlice(v);
	//	Render();
	//}
	m_MaskReslice.UpdateSlice(v);
	auto imageActor = m_View->GetMaskImageActor();
	//imageActor->SetImageData(m_MaskReslice.GetImageData());
	imageActor->Refresh();

	m_Reslice.UpdateSlice(v);
	m_View->Render(m_Reslice.GetImageData());

	auto tools = m_View->GetTools();

	tools->ForEach([&](BaseSliceTools* t) {
		t->SetCurIndex(v);
	});

	/*auto model = gModelMgr->GetModel<LandMarkListModel>();
	model->OnFrameChanged(m_View, v);*/
	gModelMgr->OnFrameChanged(m_View, v);

	if (OnSliceValueChangedCallback != nullptr)
	{
		if (!m_IgnoreOnceSliceValueChanged)
		{
			OnSliceValueChangedCallback(this, v);
		}
		m_IgnoreOnceSliceValueChanged = false;
	}
}

void SliceComponent::OnMouseEvent(MouseEventType type)
{
	auto model = gModelMgr->GetModel<MprPositionModel>();
	CoordHelper::ImageXYToVoxPos(model->m_ImageXY, m_Reslice.GetResliceMatrix(), model->m_VoxPos);

	if (type == MouseEventType_WheelBackward)
	{
		ui->spinBox->setValue(ui->spinBox->value() + 1);
	}
	else if (type == MouseEventType_WheelForward)
	{
		ui->spinBox->setValue(ui->spinBox->value() - 1);
	}

	if (OnMouseEventCallback != nullptr)
	{
		OnMouseEventCallback(this, type);
	}
}

void SliceComponent::on_playButton_clicked(bool v)
{
	if (!v) return;

	int max = ui->horizontalSlider->maximum();
	int min = ui->horizontalSlider->minimum();
	
	QElapsedTimer time;

	while (true)
	{
		auto isPlay = ui->playButton->isChecked();

		if (!isPlay)
		{
			break;
		}

		if (time.elapsed() % g_ConfigVal.PlayInterval == 0)
		{
			int next = (ui->horizontalSlider->value() + 1) % (max + 1);
			
			ui->horizontalSlider->setSliderPosition(next);
		}

		QCoreApplication::processEvents();
	}
}

void SliceComponent::resizeEvent(QResizeEvent *event)
{
	//if (!event) return;

	//int width = ui->label->size().width();
	//int height = ui->label->size().height();
	//m_RenderEngine->SetRenWinSize(width, height);
}
