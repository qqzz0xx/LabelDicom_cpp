#include "MainWindow.h"
#include "OpenFileDialog.h"
#include "AppMain.h"
#include "ResliceActor.h";
#include "ModelManager.h"
#include "ColorLabelTableModel.h"
#include "LocationTools.h"
#include "LandMarkTools.h"
#include "LandMarkListModel.h"
#include "PolygonTools.h"
#include "GlobalVal.h"
#include "LoadDroppedFileDialog.h"
#include "MaskEditRecordModel.h"
#include "BoundingBoxTools.h"
#include "BoundingBoxLabelModel.h"
#include "BoundingBox3DTools.h"
#include "BoundingBox3DLabelModel.h"
#include "LabelDetailModel.h"
#include "Helper.h"
#include "LabelDetailView.h"
#include "NThreadPool.h"
#include "LoadMgr.h"
#include "CodingHelper.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMessageBox>
#include <QFileDialog>
#include <QTemporaryDir>

#include "opencv2/opencv.hpp"
#include "gdcmReader.h"
#include "gdcmFile.h"
#include "gdcmImageHelper.h"
#include "gdcmImage.h"
#include "gdcmStreamImageReader.h"
#include "gdcmFileMetaInformation.h"
#include "gdcmPixelFormat.h"
#include "gdcmImageReader.h"
#include "gdcmPixmapReader.h"

#include "NThreadPool.h"

#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"

class RandomSliceBoxTools;

MainWindow::MainWindow(QWidget* parent /*= 0*/) :
	QMainWindow(parent),
	ui(new Ui_MainWindow)
{

	for (size_t i = 0; i < 3; i++)
	{
		m_RandomSlice[i] = nullptr;
	}

	setAcceptDrops(true);

	this->setWindowIcon(QIcon(":/icon/icon/rayshape_anno_logo_null.png"));
	ui->setupUi(this);
	ui->stackedWidget_2->setCurrentWidget(ui->startPage);
	ui->stackedWidget->setCurrentWidget(ui->pageLocation);
	//this->addAction(ui->actionClose);

	m_LabelEditorDialog = new LabelEditorDialog(this);
	auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	m_LabelEditorDialog->SetModel(model->GetModel());

	auto setComboxModelFunc = [&](auto comboBox, auto model)
	{
		if (model == nullptr) return;

		auto treeView = new QTableView(this);
		comboBox->setView(treeView);
		comboBox->setModel(model);

		treeView->setModel(model);
		treeView->verticalHeader()->hide();
		//treeView->horizontalHeader()->hide();
		treeView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
		treeView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
		treeView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
		treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
		treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	};

	setComboxModelFunc(ui->comboBox, model->GetProxyModel());
	setComboxModelFunc(ui->comboBox_2, model->GetParentModel());

	m_LabelSelectDialog = new LabelSelectDialog(this);
	m_LabelSelectDialog->SetModel(model->GetProxyModel());

	m_LabelListComponent = ui->labelList;
	auto model1 = gModelMgr->GetModel<LandMarkListModel>();
	m_LabelListComponent->SetLandMarkModel(model1->GetModel());
	auto model2 = gModelMgr->GetModel<BoundingBox3DLabelModel>();
	m_LabelListComponent->SetBoundingBoxModel(model2->GetModel());

	m_StatusLabel = new QLabel(this);
	auto str = QStringLiteral("屏幕坐标 : (%1,%2) || 世界坐标 : (%3,%4,%5) || 图像坐标 : (%6,%7) || 体坐标 : (%8,%9,%10)");
	m_StatusLabel->setText("");
	m_StatusLabel->setMargin(0);
	ui->statusbar->addPermanentWidget(m_StatusLabel);

	m_OpenFileNameLabel = new QLabel(this);
	m_OpenFileNameLabel->setStyleSheet("color:#85dd35");
	ui->statusbar->addWidget(m_OpenFileNameLabel);


	connect(ui->comboBox, &QComboBox::currentTextChanged, this, &MainWindow::OnColorLabelSelected);
	connect(ui->comboBox_2, &QComboBox::currentTextChanged, this, &MainWindow::OnColorLabelParentSelected);

	connect(this, &MainWindow::OnToolsClicked, [&](auto name, bool v) {
		if (name != typeid(RandomSliceBoxTools*).name())
		{
			if (g_GlobalVal.IsInRandomSlice)
			{
				SetEnableRandomSlice(false);
			}
		}
	});

	OnColorLabelSelected(QString::number(0));

	ui->actionLabelDetail->setEnabled(true);
	ui->label_3->setVisible(false);
	ui->filterEdit->setVisible(false);
	ui->selectParentLabelName->setText(tr(""));
	ui->actionAnySlice->setEnabled(true);
}

MainWindow::~MainWindow()
{
	if (m_MprComponent != nullptr)
	{
		delete m_MprComponent;
		m_MprComponent = nullptr;
	}
	if (m_FrameComponent != nullptr)
	{
		delete m_FrameComponent;
		m_FrameComponent = nullptr;
	}

	//for (size_t i = 0; i < 3; i++)
	//{
	//	if (m_RandomSlice[i] != nullptr)
	//	{
	//		delete m_RandomSlice[i];
	//		m_RandomSlice[i] = nullptr;
	//	}
	//}

}

void MainWindow::SetToolsEnable(const std::string& className, bool v)
{
	emit OnToolsClicked(className, v);
}

void MainWindow::RenderAll()
{
	if (m_MprComponent)
	{
		m_MprComponent->RefreshMask();
		m_MprComponent->Render();
	}
	if (m_FrameComponent)
	{
		m_FrameComponent->GetSliceView()->GetMaskImageActor()->Refresh();
		m_FrameComponent->GetSliceView()->Render();
	}
}

int MainWindow::GetCurLabelType()
{
	auto str = ui->comboBox->currentText();
	return str.toInt();
}

std::shared_ptr<ColorLabelItem> MainWindow::GetCurLabelTypeItem()
{
	auto str = ui->comboBox->currentText();
	auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	auto label = model->GetColorLabel(str);

	if (label == nullptr)
	{
		label = model->GetColorLabel(QString::number(0));
	}

	return label;
}

bool MainWindow::GetIsInvertPolygon()
{
	return ui->ckbInvert->isChecked();
}

SliceView* MainWindow::GetSliceView(MprType type)
{
	if (m_FrameComponent != nullptr && type == MprType::None)
	{
		return m_FrameComponent->GetSliceView();
	}
	else
	{
		if (m_MprComponent != nullptr)
		{
			auto sc = m_MprComponent->GetSliceComponent(type);
			if (sc)
			{
				return sc->GetSliceView();
			}
		}
	}

	return nullptr;
}

int* MainWindow::GetCurSliceVal()
{
	if (m_MprComponent != nullptr)
	{
		auto v = m_MprComponent->GetCurSliceVal();
		m_SliceVals[0] = v[0];
		m_SliceVals[1] = v[1];
		m_SliceVals[2] = v[2];
	}
	if (m_FrameComponent != nullptr)
	{
		m_SliceVals[3] = m_FrameComponent->GetCurFrameIndex();
	}

	return m_SliceVals;
}

void MainWindow::ForEachRandomSlice(std::function<void(RandomSlice*)> func)
{
	for (size_t i = 0; i < 3; i++)
	{
		auto s = m_RandomSlice[i];
		if (s && func)
		{
			func(s);
		}
	}
}

void MainWindow::on_actionImportMask_triggered()
{
	auto dialog = new OpenFileDialog(this);
	dialog->setWindowTitle(tr("Import Mask"));
	QDialog::DialogCode res = (QDialog::DialogCode)dialog->exec();
	QString path = dialog->GetFileName();
	if (res == QDialog::Accepted && !path.isEmpty())
	{
		LoadMaskData(path, dialog->IsEncryption());
	}

	delete dialog;
}

static QString g_SavePath;

void MainWindow::on_actionSave_triggered()
{
	//if (g_SavePath.isEmpty())
	{
		g_SavePath = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
	}

	//QFileInfo info(m_OpenFileNameLabel->text());
	auto path = g_SavePath;

	if (path != nullptr)
	{
		/*auto model = gModelMgr->GetModel<LandMarkListModel>();
		model->SaveToJson(path);*/
		gModelMgr->ExportJson(path);
		gLoadMgr->SaveMaskData(path);
	}
}

void MainWindow::on_landmarkButton_clicked(bool v)
{
	ui->stackedWidget->setCurrentWidget(ui->pageLandMark);
	emit OnToolsClicked(typeid(LandMarkTools*).name(), v);
}

void MainWindow::on_polygonButton_clicked(bool v)
{
	ui->stackedWidget->setCurrentWidget(ui->pagePoly);
	emit OnToolsClicked(typeid(PolygonTools*).name(), v);
}

void MainWindow::on_actionLabelEditor_triggered()
{
	m_LabelEditorDialog->show();
}

void MainWindow::on_actionClose_triggered()
{
	ui->stackedWidget_2->setCurrentWidget(ui->startPage);
	m_OpenFileNameLabel->setText("");
	m_StatusLabel->setText("");

	if (m_OpenDirWidget)
	{
		m_OpenDirWidget->setVisible(false);
	}
}

void MainWindow::on_actionAnySlice_triggered()
{
	if (m_MprComponent == nullptr) return;

	g_GlobalVal.IsInRandomSlice = !g_GlobalVal.IsInRandomSlice;

	for (size_t i = 0; i < 3; i++)
	{
		auto sc = m_MprComponent->GetSliceComponent((MprType)i);

		bool isShow = m_RandomSlice[i]->isVisible();
		m_RandomSlice[i]->setVisible(g_GlobalVal.IsInRandomSlice);
		sc->setVisible(!g_GlobalVal.IsInRandomSlice);
	}

	//m_RandomSliceDock->show();
}

void MainWindow::on_actionImportJson_triggered()
{
	auto dialog = new OpenFileDialog(this);
	dialog->setWindowTitle(tr("Import Label"));
	dialog->FilterString = tr("*.json *.conf *.cdat");
	QDialog::DialogCode res = (QDialog::DialogCode)dialog->exec();
	QString path = dialog->GetFileName();
	if (res == QDialog::Accepted && !path.isEmpty())
	{
		gModelMgr->ImportJson(path, dialog->IsEncryption());
	}

	delete dialog;
}

void MainWindow::on_actionLabelDetail_triggered()
{

	if (m_LabelDetailView == nullptr)
	{
		m_LabelDetailView = new LabelDetailView(this);
		m_LabelDetailView->setVisible(false);
		m_LabelDetailView->SetModel(gModelMgr->GetModel<LabelDetailModel>()->GetModel());
	}

	if (!m_LabelDetailView->isVisible())
	{
		m_LabelDetailView->show();

		auto dock = new QDockWidget(this);
		dock->setWidget(m_LabelDetailView);
		dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable
			| QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
		dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
		addDockWidget(Qt::BottomDockWidgetArea, dock);
		dock->setVisible(true);

		connect(dock, &QDockWidget::topLevelChanged, [dw = dock](bool v) {
		});
	}
}

void MainWindow::on_actionLoadDir_triggered()
{
	auto dialog = new OpenFileDialog(this);
	dialog->setWindowTitle(tr("Load Dir..."));
	dialog->SetOpenDir(true);
	QDialog::DialogCode res = (QDialog::DialogCode)dialog->exec();
	QString path = dialog->GetFileName();

	if (res == QDialog::Accepted && !path.isEmpty())
	{
		//auto file = QFileInfo(path);
		QList<int> filter;
		filter << dialog->GetFileType();
		LoadDir(path, filter);
	}
}

void MainWindow::on_actionAbout_triggered()
{
	auto path = gAppMain->applicationDirPath() + "/about.txt";
	QFile f(path);
	f.open(QFile::ReadOnly);

	if (!f.isReadable())
	{
		f.close();
		return;
	}

	auto bytes = f.readAll();
	f.close();

	QMessageBox mb(QMessageBox::Information, "Tips",
		QString::fromUtf8(bytes), QMessageBox::Ok, nullptr);
	mb.setTextInteractionFlags(Qt::TextSelectableByMouse);
	mb.exec();
}

void MainWindow::on_navLocation_clicked(bool v)
{
	ui->stackedWidget->setCurrentWidget(ui->pageLocation);
	emit OnToolsClicked(typeid(LocationTools*).name(), v);
}

void MainWindow::on_pushButton_3_clicked(bool v)
{
	ui->stackedWidget->setCurrentWidget(ui->pageRect);
	emit OnToolsClicked(typeid(BoundingBoxTools*).name(), v);
}

void MainWindow::on_btnRedo_clicked()
{
	auto model = gModelMgr->GetModel<MaskEditRecordModel>();
	model->Redo();
	RenderAll();
}

void MainWindow::on_btnUndo_clicked()
{
	auto model = gModelMgr->GetModel<MaskEditRecordModel>();
	model->Undo();
	RenderAll();
}

void MainWindow::on_inOpacitySlider_valueChanged(int v)
{
	auto model = gModelMgr->GetModel<ColorLabelTableModel>();

	auto func = [dialog = m_LabelEditorDialog, v, model](auto label)
	{
		label->Color.setAlpha(v);
		model->UpdateColorLabel(label);
		dialog->UpdateRightInfo(label);
		emit  dialog->LabelTypeChanged(label.get());
	};

	if (ui->allLabelCheckBox->isChecked())
	{
		model->ForEach([&](auto label) {
			func(label);
		});
	}
	else
	{
		auto label = model->GetColorLabel(QString::number(GetCurLabelType()));
		func(label);
	}
}

void MainWindow::on_rectDelBtn_clicked(bool v)
{
	auto model = gModelMgr->GetModel<BoundingBoxLabelModel>();
	model->RemoveSelected();
}

void MainWindow::on_rectDrawBtn_clicked(bool v)
{
	auto model = gModelMgr->GetModel<BoundingBoxLabelModel>();
	model->SetDrawEnble(v);

}

void MainWindow::on_box3DButton_clicked(bool v)
{
	ui->stackedWidget->setCurrentWidget(ui->pageBox3D);
	emit OnToolsClicked(typeid(BoundingBox3DTools*).name(), v);
}

void MainWindow::on_rectDelBtn_2_clicked(bool v)
{
	auto model = gModelMgr->GetModel<BoundingBox3DLabelModel>();
	model->RemoveSelected();
}

void MainWindow::on_rectDrawBtn_2_clicked(bool v)
{
	auto model1 = gModelMgr->GetModel<BoundingBox3DLabelModel>();
	model1->SetDrawEnble(v);
}

void MainWindow::on_box3DPlusBtn_clicked(bool v)
{
	SetEnableRandomSlice(v);
}

void MainWindow::on_filterEdit_textChanged(const QString& txt)
{
	//auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	//model->GetProxyModel()->setFilterKeyColumn(2);
	//model->GetProxyModel()->setFilterFixedString(txt);
}

void MainWindow::OnColorLabelSelected(const QString& text)
{
	auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	auto label = model->GetColorLabel(text);
	if (label == nullptr) return;

	auto icon = QtHelper::CreateColorBoxIcon(25, 25, label->Color);

	ui->inSelectColor->setPixmap(icon);
	ui->inOpacitySpinBox->blockSignals(true);
	ui->inOpacitySpinBox->setValue(label->Color.alpha());
	ui->inOpacitySpinBox->blockSignals(false);
	ui->inOpacitySlider->blockSignals(true);
	ui->inOpacitySlider->setValue(label->Color.alpha());
	ui->inOpacitySlider->blockSignals(false);
	ui->selectLabelName->setText(label->Desc);
}

void MainWindow::OnColorLabelParentSelected(const QString& text)
{
	auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	model->OnParentLabelChanged(text);

	auto label = model->GetColorLabel(text);
	if (label == nullptr) return;

	ui->selectParentLabelName->setText(label->Desc);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* md = event->mimeData();
	if (md->hasUrls() && md->urls().size() == 1)
	{
		QUrl url = md->urls().first();
		if (url.isLocalFile())
		{
			event->setDropAction(Qt::CopyAction);
			event->accept();
		}
	}
}

void MainWindow::dropEvent(QDropEvent* event)
{
	QUrl url = event->mimeData()->urls().first();
	QString file = url.toLocalFile();
	event->acceptProposedAction();

	auto info = QFileInfo(file);
	if (info.isDir())
	{
		OpenFileDialog::SetFileName(std::move(file));
		on_actionLoadDir_triggered();
	}
	else
	{
		auto suffix = info.suffix();
		if (suffix == "json" || suffix == "conf")
		{
			gModelMgr->ImportJson(file, false);
		}
		else if (suffix == "cdat")
		{
			gModelMgr->ImportJson(file, true);
		}
		else
		{
			LoadDroppedFile(file);
		}
	}
}

void MainWindow::LoadDroppedFile(const QString& file)
{
	auto dialog = new LoadDroppedFileDialog(this);
	auto reply = dialog->exec();
	if (reply == QDialog::Accepted)
	{
		auto isLoadMainImage = dialog->IsLoadMainImage();
		auto isEncryption = dialog->IsEncryption();
		if (isLoadMainImage)
		{
			LoadMainImage(file, isEncryption);
		}
		else
		{
			LoadMaskData(file, isEncryption);
		}
	}

	delete dialog;
}

void MainWindow::LoadMainImage(const QString& path, bool encryption)
{
	gModelMgr->Exit();
	g_GlobalVal.IsPrjectChanged = false;

	try
	{
		gLoadMgr->LoadFile(path, encryption);
	}
	catch (...)
	{
		QMessageBox mb(QMessageBox::Information, "Tips",
			QStringLiteral("解析出错"), QMessageBox::Ok, nullptr);
		mb.setTextInteractionFlags(Qt::TextSelectableByMouse);
		mb.exec();
		return;
	}

	auto type = gLoadMgr->GetImageFileType();
	if (type == ImageFileType_None)
	{
		QMessageBox mb(QMessageBox::Information, "Tips",
			QStringLiteral("只支持: *.png *.jpg *.bmp *.mhd *.nii.gz *nii *.dcm *.mp4 *.avi"), QMessageBox::Ok, nullptr);
		mb.setTextInteractionFlags(Qt::TextSelectableByMouse);
		mb.exec();
		return;
	}

	if (m_MprComponent != nullptr)
	{
		delete m_MprComponent;
		m_MprComponent = nullptr;
	}
	if (m_FrameComponent != nullptr)
	{
		delete m_FrameComponent;
		m_FrameComponent = nullptr;
	}

	for (size_t i = 0; i < 3; i++)
	{
		if (m_RandomSlice[i] != nullptr)
		{
			//delete m_RandomSlice[i];
			m_RandomSlice[i] = nullptr;
		}
	}

	if (m_RandomSliceDock != nullptr)
	{
		delete m_RandomSliceDock;
		m_RandomSliceDock = nullptr;
	}

	if (type == ImageFileType_SequenceFrame || type == ImageFileType_SingleFrame)
	{
		m_SliceType = SliceType::SliceType_FrameSlice;

		m_FrameComponent = new FrameComponent(this);
		ui->widget->layout()->addWidget(m_FrameComponent);

		m_FrameComponent->Init(gLoadMgr->GetFrameByIndex(0));
	}
	else
	{
		m_SliceType = SliceType::SliceType_VoxSlice;

		m_MprComponent = new MprComponent(this);
		ui->widget->layout()->addWidget(m_MprComponent);

		m_MprComponent->Init(gLoadMgr->GetImageData());

		for (size_t i = 0; i < 3; i++)
		{
			m_RandomSlice[i] = new RandomSlice(this);
			m_RandomSlice[i]->Init(gLoadMgr->GetImageData(), MprType(4 + i));
			m_RandomSlice[i]->setVisible(false);

		}

		m_MprComponent->ui->gridLayout->addWidget(m_RandomSlice[0], 0, 0);
		m_MprComponent->ui->gridLayout->addWidget(m_RandomSlice[1], 0, 1);
		m_MprComponent->ui->gridLayout->addWidget(m_RandomSlice[2], 1, 0);

		auto& tags = gLoadMgr->GetDicomTags();
		auto all = ui->groupBox_2->children();
		qDeleteAll(all);
		auto layout = new QVBoxLayout(ui->groupBox_2);
		for (auto&& tag : tags)
		{
			layout->addWidget(new QLabel(tag.second, this));
		}
	}

	ui->stackedWidget_2->setCurrentWidget(ui->mainPage);
	ui->navLocation->click();

	m_OpenFileNameLabel->setText(path);

	gModelMgr->Start();
}

//void MainWindow::LoadMainImageAsync(const QString& path, bool encryption)
//{
//	auto t = new NThread;
//	t->Callback = [&]() {
//		LoadMainImage(path, encryption);
//	};
//	t->start();
//
//	gAppMain->ShowLoading(t);
//}

void MainWindow::LoadMaskData(const QString& path, bool encryption)
{
	bool ok;
	try
	{
		ok = gLoadMgr->LoadMaskData(path, encryption);

	}
	catch (const std::exception&)
	{
		QMessageBox mb(QMessageBox::Information, "Tips",
			QStringLiteral("解析出错"), QMessageBox::Ok, nullptr);
		mb.setTextInteractionFlags(Qt::TextSelectableByMouse);
		mb.exec();
		return;
	}

	if (ok)
	{
		if (m_MprComponent != nullptr)
		{
			m_MprComponent->RefreshMask();
			m_MprComponent->Render();
		}
		if (m_FrameComponent != nullptr)
		{
			auto imageActor = m_FrameComponent->GetSliceView()->GetMaskImageActor();
			imageActor->Refresh();
			imageActor->SetActive(true);
			m_FrameComponent->GetSliceView()->Render();
		}
	}
	else
	{
		QMessageBox mb(QMessageBox::Information, "Tips",
			QStringLiteral("格式错误或者不是对应的mask:\n *.nii.gz *nii *.dcm"), QMessageBox::Ok, nullptr);
		mb.setTextInteractionFlags(Qt::TextSelectableByMouse);
		mb.exec();
	}
}

QImage ConvertToFormat_RGB888(uint8* imagePtr, int* dimension)
{
	unsigned int dimX = dimension[0];
	unsigned int dimY = dimension[1];

	// We need to copy each individual 16bits into R / G and B (truncate value)
	unsigned char* ubuffer = new unsigned char[dimX * dimY * 3];
	unsigned char* pubuffer = ubuffer;
	for (unsigned int i = 0; i < dimX * dimY; i++)
	{
		*pubuffer++ = std::min((uint8)255, imagePtr[0]);
		*pubuffer++ = std::min((uint8)255, imagePtr[1]);
		*pubuffer++ = std::min((uint8)255, imagePtr[2]);
		imagePtr += 3;
	}

	auto image = QImage(ubuffer, dimX, dimY, QImage::Format_RGB888);
	delete[] ubuffer;
	return image;
}

void MainWindow::LoadDir(const QString& path, const QList<int>& filter)
{
	QDir dir(path);

	auto size = QSize(128, 128);

	if (m_OpenDirWidget == nullptr)
	{
		auto listWidget = new QListWidget(this);
		listWidget->setViewMode(QListView::IconMode);
		listWidget->setFlow(QListView::TopToBottom);
		listWidget->setWrapping(false);
		listWidget->setIconSize(size);
		listWidget->setDragDropMode(QListWidget::NoDragDrop);
		listWidget->setSpacing(10);
		listWidget->setMinimumWidth(size.width() + 50);
		listWidget->setMaximumWidth(size.width() + 50);

		auto dock = new QDockWidget(this);
		dock->setWindowTitle(tr("Files"));
		dock->setWidget(listWidget);
		dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
		dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
		addDockWidget(Qt::LeftDockWidgetArea, dock);
		dock->setVisible(true);

		m_OpenDirWidget = listWidget;

		connect(m_OpenDirWidget, &QListWidget::doubleClicked, [&](const QModelIndex& idx) {
			if (!idx.isValid()) return;
			auto p = (QListWidgetItemWithPath*)m_OpenDirWidget->item(idx.row());
			auto name = p->Path;
			bool b = name.contains("_encrypt") && !name.contains("_decrypt") ? true : false;
			LoadMainImage(p->Path, b);

		});
	}
	else
	{
		m_OpenDirWidget->clear();
	}

	auto files = dir.entryInfoList(QDir::Files);
	for (auto&& file : files)
	{
		auto item = new QListWidgetItemWithPath;
		auto icon = QPixmap(":icon/icon/picture.png");

		auto fullPath = file.absoluteFilePath();
		item->Path = fullPath;

		auto filePathStr = fullPath.toStdString();
		bool isEncrypt = fullPath.contains("_encrypt") ? true : false;
		auto suffix = file.completeSuffix();
		auto type = LoadMgr::CheckImageType(suffix);
		auto showtype = Helper::FileTypeToShowType(type);

		if (type == ImageFileType_None
			|| (!filter.isEmpty() && !filter.contains(showtype))) continue;

		try
		{
			if (type == ImageFileType_SequenceFrame)
			{
				icon = QPixmap(":icon/icon/video-player.png");

				cv::VideoCapture cp;

				if (isEncrypt)
				{
					auto path = g_GlobalVal.TempDir.path();
					auto temp = CodingHelper::DeCodingAES(file, path);
					filePathStr = temp.toStdString();
					item->Path = temp;
				}
				cp.open(filePathStr);
				cv::Mat mat;
				cp >> mat;
				auto image = Helper::mat2qim(mat);
				icon = QPixmap::fromImage(image);

			}
			else if (type == ImageFileType_SingleFrame)
			{
				icon = QPixmap(":icon/icon/picture.png");


				auto mat = cv::imread(filePathStr);
				auto image = Helper::mat2qim(mat);
				icon = QPixmap::fromImage(image);

			}
			else if (type == ImageFileType_VolumeDicom)
			{

				auto task = [fullPath, size, item]() {

					using ImageType = itk::Image<uint8, 3>;
					auto reader = itk::ImageFileReader< ImageType >::New();
					using ImageIOType = itk::GDCMImageIO;
					ImageIOType::Pointer dicomIO = ImageIOType::New();

					reader->SetFileName(fullPath.toLocal8Bit().toStdString());
					reader->SetImageIO(dicomIO);
					reader->Update();

					auto image = reader->GetOutput();
					auto ptr = image->GetBufferPointer();
					auto dim = image->GetLargestPossibleRegion().GetSize();
					Helper::NormalizBuffer(image->GetBufferPointer(), dim[0] * dim[1]);
					auto qimage = QImage(image->GetBufferPointer(), dim[0], dim[1], QImage::Format_Grayscale8);
					auto icon = QPixmap::fromImage(qimage);
					icon = icon.scaled(size - QSize(30, 30), Qt::KeepAspectRatioByExpanding);
					item->setIcon(icon);
				};

				gAppMain->ThreadPool->Start(task);

			}
		}
		catch (const std::exception & ex)
		{
			qDebug() << ex.what();
		}

		icon = icon.scaled(size - QSize(30, 30), Qt::KeepAspectRatioByExpanding);
		item->setIcon(icon);
		item->setText(file.fileName());
		item->setToolTip(file.fileName());
		item->setSizeHint(size);
		item->setTextAlignment(Qt::AlignVCenter);


		m_OpenDirWidget->addItem(item);

	}

	m_OpenDirWidget->setVisible(true);
}

void MainWindow::SetEnableRandomSlice(bool v)
{
	if (m_MprComponent == nullptr) return;

	g_GlobalVal.IsInRandomSlice = v;

	for (size_t i = 0; i < 3; i++)
	{
		auto sc = m_MprComponent->GetSliceComponent((MprType)i);

		bool isShow = m_RandomSlice[i]->isVisible();
		m_RandomSlice[i]->setVisible(g_GlobalVal.IsInRandomSlice);
		sc->setVisible(!g_GlobalVal.IsInRandomSlice);
	}
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	if (event)
	{
		if (event->key() == Qt::Key_F1)
		{
			auto reply = m_LabelSelectDialog->exec();
			if (reply == QDialog::Accepted)
			{
				auto label = m_LabelSelectDialog->value();
				auto index = ui->comboBox->findText(label->ID);
				if (index != -1) { // -1 for not found
					ui->comboBox->setCurrentIndex(index);
				}
			}
		}
	}
}

void MainWindow::on_actionLoad_triggered()
{
	auto dialog = new OpenFileDialog(this);
	dialog->setWindowTitle(tr("Load ..."));
	QDialog::DialogCode res = (QDialog::DialogCode)dialog->exec();
	QString path = dialog->GetFileName();

	if (res == QDialog::Accepted && !path.isEmpty())
	{
		LoadMainImage(path, dialog->IsEncryption());
	}
}
