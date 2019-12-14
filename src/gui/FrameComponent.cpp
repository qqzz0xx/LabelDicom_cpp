#include "FrameComponent.h"
#include "AppMain.h"
#include "CoordHelper.h"
#include "MprPositionModel.h"
#include "LocationTools.h"
#include "WinLevelWidthTools.h"
#include "LandMarkTools.h"
#include "PolygonTools.h"
#include "BoundingBoxTools.h"
#include "CameraZoomAndMoveTools.h"
#include "LandMarkListModel.h"
#include "ColorLabelTableModel.h"
#include "FrameLabelModel.h"
#include "BoundingBoxLabelModel.h"
#include "BoundingBox3DLabelModel.h"
#include "LoadMgr.h"
#include "vtkImageResize.h"

#include <QWheelEvent>
#include <QPixmap>
#include <QTimer>
#include <QElapsedTimer>


FrameComponent::FrameComponent(QWidget* parent /* = Q_NULLPTR */, Qt::WindowFlags f /* = Qt::WindowFlags() */) :
	QWidget(parent),
	ui(new Ui_FrameComponent)
{
	ui->setupUi(this);

	ui->listWidget->setViewMode(QListView::IconMode);
	ui->listWidget->setIconSize(QSize(150, 150));
	ui->listWidget->setSpacing(5);
	ui->listWidget->setResizeMode(QListWidget::ResizeMode::Adjust);
	ui->listWidget->setMovement(QListWidget::Static);
	ui->listWidget->setWrapping(false);
	ui->listWidget->horizontalScrollBar()->setSingleStep(40);
	ui->listWidget->verticalScrollBar()->setSingleStep(40);
	ui->structLabelButton->setText(QStringLiteral("ÊÓÍ¼±ê¼Ç"));

	InitTools();

	connect(gMainWindow->m_LabelEditorDialog,
		&LabelEditorDialog::LabelTypeChanged, this, &FrameComponent::OnLabelTypeChanged);

	auto func = [&] {
		auto val = ui->listWidget->horizontalScrollBar()->value();
		auto max = ui->listWidget->horizontalScrollBar()->maximum();
		float factor = val / (float)max;

		qDebug() << "bar val: " << val;

		int maxFrame = gLoadMgr->GetMaxFrameCount();

		int curFrame = (int)(factor * (maxFrame - 1));
		if (curFrame != m_CurIndex)
		{
			auto data = gLoadMgr->GetFrameByIndex(curFrame);

			ui->listWidget->horizontalScrollBar()->blockSignals(true);
			ui->listWidget->blockSignals(true);

			RenderFrame(data, curFrame);

			ui->listWidget->blockSignals(false);
			ui->listWidget->horizontalScrollBar()->blockSignals(false);

			qDebug() << "bar changed: " << curFrame;
		}
	};

	connect(ui->listWidget->horizontalScrollBar(), &QScrollBar::sliderMoved, this, func);
	connect(ui->listWidget->horizontalScrollBar(), &QScrollBar::sliderReleased, this, func);

	ui->view->OnMouseEventCallback = [&](SliceView* view, MouseEventType ev)
	{
		OnMouseEvent(ev);
	};
}

// The actual conversion code
QImage FrameComponent::VtkImageDataToQImage(vtkImageData* imageData, int radio)
{
	if (!imageData) { return QImage(); }
	
	//radio = 1;
	/// \todo retrieve just the UpdateExtent
	int width = imageData->GetDimensions()[0] / radio;
	int height = imageData->GetDimensions()[1] / radio;

	auto resize = vtkSmartPointer<vtkImageResize>::New();
	resize->SetInputData(imageData);
	resize->SetOutputDimensions(width, height, 1);
	resize->Update();

	imageData = resize->GetOutput();

	QImage image(width, height, QImage::Format_RGB32);
	QRgb* rgbPtr =
		reinterpret_cast<QRgb*>(image.bits());
	unsigned char* colorsPtr =
		reinterpret_cast<unsigned char*>(imageData->GetScalarPointer());

	// Loop over the vtkImageData contents.
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			// Swap the vtkImageData RGB values with an equivalent QColor
			*(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
			colorsPtr += imageData->GetNumberOfScalarComponents();
		}
	}

	return image;
}

void FrameComponent::on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (!current) return;

	int idx = current->text().toInt();
	qDebug() << current;
	qDebug() << idx;

	auto data = gLoadMgr->GetFrameByIndex(idx);
	RenderFrame(data, idx);
}

FrameComponent::~FrameComponent()
{

}

void FrameComponent::Init(vtkImageData* data)
{
	auto start = std::chrono::steady_clock::now();
	
	ui->listWidget->clear();
	
	for (size_t i = 0, size = gLoadMgr->GetMaxFrameCount(); i < size; i++)
	{
		auto data = gLoadMgr->GetFrameByIndex(i);
		AddToList(data, i);
	}
	
	data = gLoadMgr->GetFrameByIndex(0);

	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	qDebug() << std::chrono::duration <double>(diff).count() << " s" << endl;

	m_CurIndex = 0;
	ui->view->Init(data, MprType::None);

	if (m_CurIndex == ui->listWidget->count())
	{
		AddToList(data, m_CurIndex);
	}

	ui->listWidget->blockSignals(true);
	ui->listWidget->setCurrentRow(m_CurIndex);
	ui->listWidget->blockSignals(false);


	auto mat = vtkSmartPointer<vtkMatrix4x4>::New();
	mat->Identity();
	mat->Element[1][1] = -1;

	auto imageActor = ui->view->GetSingleImageActor();
	imageActor->SetWorldMat(mat);
	imageActor->ResetCamera();

	auto maskActor = ui->view->GetMaskImageActor();
	maskActor->SetWorldMat(mat);

	auto maskData = gLoadMgr->GetMaskDataByIndex(0);
	auto model1 = gModelMgr->GetModel<ColorLabelTableModel>();
	auto lookup = model1->GetLookupTable();
	maskActor->SetLookupTable(lookup);
	maskActor->SetImageData(maskData);
	maskActor->Refresh();

}

void FrameComponent::OnMouseEvent(MouseEventType ev)
{
	if (ev == MouseEventType_WheelForward)
	{
		on_preFrame_clicked();
	}
	else if (ev == MouseEventType_WheelBackward)
	{
		on_nextFrame_clicked();
	}
}

void FrameComponent::RefreshIconListColor()
{
	auto model = gModelMgr->GetModel<LandMarkListModel>();
	auto& map = model->GetData();
	std::set<int> labelIndexs;

	for (auto&& [k, v]: map)
	{
		if (v->ViewType == MprType::None)
		{
			labelIndexs.emplace(v->ImageIndex);
		}
	}

	for (size_t i = 0, length = ui->listWidget->count(); i < length; i++)
	{
		auto item = ui->listWidget->item(i);
		item->setTextColor(QColor(255, 255, 255));
		QFont font;
		font.setBold(false);
		font.setItalic(false);
		font.setUnderline(false);

		item->setFont(font);
	}
	
	for (auto&& v : labelIndexs)
	{
		auto item = ui->listWidget->item(v);
		item->setTextColor(QColor(30, 197, 57));
		QFont font;
		font.setBold(true);
		font.setItalic(true);
		font.setUnderline(true);

		item->setFont(font);
	}

	auto model1 = gModelMgr->GetModel<FrameLabelModel>();
	model1->ForEach([&](std::shared_ptr<FrameLabelItem> p) {
		auto item = ui->listWidget->item(p->FrameCount);
		if (item)
		{
			item->setTextColor(p->LabelType->Color);
		}
	});
}

void FrameComponent::on_nextFrame_clicked()
{
	auto data = gLoadMgr->GetFrameByIndex(m_CurIndex + 1);
	if (data != nullptr)
	{
		RenderFrame(data, m_CurIndex + 1);
	}
}

void FrameComponent::on_preFrame_clicked()
{
	if (m_CurIndex > 0)
	{
		auto data = gLoadMgr->GetFrameByIndex(m_CurIndex - 1);
		if (data != nullptr)
		{
			RenderFrame(data, m_CurIndex - 1);
		}
	}
}

void FrameComponent::on_playFrame_clicked(bool v)
{
	if (!v) return;

	int max = gLoadMgr->GetMaxFrameCount();

	auto time = new QElapsedTimer();

	while (true)
	{
		auto isPlay = ui->playFrame->isChecked();

		if (!isPlay)
		{
			break;
		}

		if (time->elapsed() % m_PlayInterval == 0)
		{
			int next = m_CurIndex + 1;
			if (next < max)
			{
				auto data = gLoadMgr->GetFrameByIndex(next);
				RenderFrame(data, next);
			}
			else
			{
				ui->playFrame->setChecked(false);
				break;
			}
		}

		QCoreApplication::processEvents();
	}
}

void FrameComponent::on_rePlayFrame_clicked()
{
	m_CurIndex = -1;
	if (!ui->playFrame->isChecked())
	{
		ui->playFrame->click();
	}
}

void FrameComponent::on_frameLabelButton_clicked()
{
	auto model = gModelMgr->GetModel<FrameLabelModel>();
	
	auto id = gMainWindow->GetCurLabelType();

	auto colorLableModel = gModelMgr->GetModel<ColorLabelTableModel>();
	auto label = colorLableModel->GetColorLabel(QString::number(id));

	auto item = std::make_shared<FrameLabelItem>();
	item->FrameCount = m_CurIndex;
	item->ViewType = MprType::None;
	item->LabelType = label;
	item->View = ui->view;
	item->ItemType = FrameLabelType_Frame;

	model->Add(item);

	auto fc = gMainWindow->GetFrameComponent();
	if (fc)
	{
		fc->RefreshIconListColor();
	}

	ui->view->Render();
}

void FrameComponent::on_clearLabelButton_clicked()
{
	auto model = gModelMgr->GetModel<FrameLabelModel>();
	model->Remove(ui->view, m_CurIndex);

	ui->view->Render();
}

void FrameComponent::on_illnessTypeLabelButton_clicked()
{
	auto model = gModelMgr->GetModel<FrameLabelModel>();
	auto id = gMainWindow->GetCurLabelType();

	auto colorLableModel = gModelMgr->GetModel<ColorLabelTableModel>();
	auto label = colorLableModel->GetColorLabel(QString::number(id));

	auto item = std::make_shared<FrameLabelItem>();
	item->FrameCount = m_CurIndex;
	item->ViewType = MprType::None;
	item->LabelType = label;
	item->View = ui->view;
	item->ItemType = FrameLabelType_Illness;

	model->Add(item);

	ui->view->Render();
}

void FrameComponent::on_structLabelButton_clicked()
{
	auto model = gModelMgr->GetModel<FrameLabelModel>();
	auto id = gMainWindow->GetCurLabelType();

	auto colorLableModel = gModelMgr->GetModel<ColorLabelTableModel>();
	auto label = colorLableModel->GetColorLabel(QString::number(id));

	auto item = std::make_shared<FrameLabelItem>();
	item->FrameCount = m_CurIndex;
	item->ViewType = MprType::None;
	item->LabelType = label;
	item->View = ui->view;
	item->ItemType = FrameLabelType_Struct;

	model->Add(item);

	ui->view->Render();
}

void FrameComponent::OnLabelTypeChanged(ColorLabelItem* colorLabel)
{
	auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	auto lookup = model->GetLookupTable();

	auto actor = ui->view->GetMaskImageActor();
	if (actor)
	{
		actor->SetLookupTable(lookup);
		actor->Refresh();
	}

	auto model1 = gModelMgr->GetModel<BoundingBoxLabelModel>();
	model1->OnColorLabelUpdate();
	auto model2 = gModelMgr->GetModel<BoundingBox3DLabelModel>();
	model2->OnColorLabelUpdate();

	ui->view->Render();
}

void FrameComponent::InitTools()
{
	auto tools = ui->view->GetTools();

	auto func = [&](auto t)
	{
		t->Init(ui->view);
		tools->Add(t);
	};
	auto zoomTools = std::make_shared<CameraZoomAndMoveTools>();
	zoomTools->ZoomChangedCallback = [&](double v) {
		OnZoomChanged(v);
	};

	func(std::make_shared<LocationTools>());
	func(std::make_shared<WinLevelWidthTools>());
	func(std::make_shared<LandMarkTools>());
	func(std::make_shared<PolygonTools>());
	func(std::make_shared<BoundingBoxTools>());
	func(zoomTools);

}

void FrameComponent::RenderFrame(vtkImageData* data, int index)
{
	m_CurIndex = index;

	auto tools = ui->view->GetTools();

	tools->ForEach([&](BaseSliceTools* t){
		t->SetCurIndex(index);
	});

	//auto model = gModelMgr->GetModel<LandMarkListModel>();
	//model->OnFrameChanged(ui->view, index);
	gModelMgr->OnFrameChanged(ui->view, index);

	auto maskActor = ui->view->GetMaskImageActor();
	auto maskData = gLoadMgr->GetMaskDataByIndex(index);
	auto model1 = gModelMgr->GetModel<ColorLabelTableModel>();
	auto lookup = model1->GetLookupTable();
	maskActor->SetLookupTable(lookup);
	maskActor->SetImageData(maskData);
	maskActor->Refresh();

	ui->view->Render(data);

	if (m_CurIndex == ui->listWidget->count())
	{
		AddToList(data, index);
	}

	ui->listWidget->blockSignals(true);
	ui->listWidget->setCurrentRow(m_CurIndex);
	ui->listWidget->blockSignals(false);
}

void FrameComponent::AddToList(vtkImageData* data, int index)
{
	auto item = new QListWidgetItem;

	auto image = VtkImageDataToQImage(data, 5);
	auto pixmap = QPixmap::fromImage(image);

	item->setIcon(pixmap);
	item->setText(QString::number(index));

	ui->listWidget->addItem(item);
}

void FrameComponent::resizeEvent(QResizeEvent* event)
{

}

void FrameComponent::OnZoomChanged(double factor)
{
	auto re = ui->view->GetRenderEngine();
	auto& nodelist = re->GetSceneManager()->GetGlobleScene()->NodeList();
	for (auto&& var : nodelist)
	{
		var->OnCameraZoom(factor);
	}
}
