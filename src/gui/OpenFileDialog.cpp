#include "OpenFileDialog.h"
#include "LoadMgr.h"
#include <QFileDialog>

static QString s_FilePath;

OpenFileDialog::OpenFileDialog(QWidget* parent /*= Q_NULLPTR*/, Qt::WindowFlags f /*= Qt::WindowFlags()*/):
	QDialog(parent),
	ui(new Ui_OpenFileDialog)
{
	FilterString = tr("Image Files (*.png *.jpg *.bmp *.mhd *.nii.gz *nii *.dcm *.mp4 *.avi *.vdat *.*)");
	ui->setupUi(this);

	ui->comboBox->addItem("Image");
	ui->comboBox->addItem("Video");
	ui->comboBox->addItem("Volume");

	ui->lineEdit->setText(s_FilePath);
}

OpenFileDialog::~OpenFileDialog()
{
}

QString OpenFileDialog::GetFileName()
{
	return ui->lineEdit->text();
}

bool OpenFileDialog::IsEncryption()
{
	return ui->checkBox->isChecked();
}

void OpenFileDialog::SetFileName(QString&& name)
{
	s_FilePath = name;
	//ui->lineEdit->setText(name);
}

int OpenFileDialog::GetFileType()
{
	return ui->comboBox->currentIndex();
}

void OpenFileDialog::on_browse_clicked()
{
	QString fileName;

	if (m_IsOpenDir)
	{
		fileName = QFileDialog::getExistingDirectory(this,
			tr("Open Dir"), s_FilePath);
	}
	else
	{
		fileName = QFileDialog::getOpenFileName(this,
			tr("Open Image"), s_FilePath, FilterString);


		auto info = QFileInfo(fileName);
		auto type = LoadMgr::CheckImageType(info.completeSuffix());
		switch (type)
		{
		case ImageFileType_None:
			break;
		case ImageFileType_NiTFI:
		case ImageFileType_MetaImage:
		case ImageFileType_VolumeDicom:
			ui->comboBox->setCurrentIndex(2);
			break;
		case ImageFileType_SingleFrame:
			ui->comboBox->setCurrentIndex(0);
			break;
		case ImageFileType_SequenceFrame:
			ui->comboBox->setCurrentIndex(1);
			break;
		default:
			break;
		}

	}
	
	if (fileName.isEmpty()) return;

	s_FilePath = fileName;

	ui->lineEdit->setText(fileName);
}


