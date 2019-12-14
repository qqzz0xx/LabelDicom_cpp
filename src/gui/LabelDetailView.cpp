#include "LabelDetailView.h"
#include "Helper.h"
#include "LabelDetailModel.h"
#include "ModelManager.h"
#include "AppMain.h"

#include <QFileDialog>
#include <QGroupBox>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QTextEdit>
#include <QSplitter>
#include <QHBoxLayout>
#include <QSizePolicy>
static QString s_ExportPath;

LabelDetailView::LabelDetailView(QWidget* parent /*= Q_NULLPTR*/) :
	QWidget(parent),
	ui(new Ui_LabelDetailView)
{
	ui->setupUi(this);

	ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
	ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);

	auto splitter = new QSplitter(Qt::Horizontal, this);
	ui->widget->setParent(splitter);
	ui->scrollArea->setParent(splitter);
	this->layout()->addWidget(splitter);

	auto layout = new QHBoxLayout(this);
	splitter->setLayout(layout);

	layout->addWidget(ui->widget);
	layout->addWidget(ui->scrollArea);
	layout->setStretch(0, 2);
	layout->setStretch(1, 1);

	connect(ui->tableView, &QTableView::clicked, this, &LabelDetailView::OnSelectTable);
}

void LabelDetailView::LoadOption(const nlohmann::json& j)
{
	if (j.is_null()) return;

	auto all = ui->scrollAreaWidgetContents->children();
	qDeleteAll(all);
	auto ContentLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);

	//auto model = gModelMgr->GetModel<LabelDetailModel>();
	//auto headers = model->GetTableHeader();

	for (auto&& var : j)
	{
		auto title = QString::fromStdString(var["title"]);

		auto box = new QGroupBox(this);
		box->setTitle(title);
		auto layout = new QGridLayout(box);

		nlohmann::json lineNum = 1;
		JsonHelper::TryGetValue(var, "lineNum", lineNum);

		auto inputType = var["inputType"];
		if (inputType == "radio")
		{
			int i = 0;
			int row = 0;
			int rowMax = lineNum;
			auto _j = var["values"];
			int colNum = std::ceil(_j.size() / (float)rowMax);

			for (auto&& op : _j)
			{
				int row = i / colNum;
				auto radio = new QRadioButton(QString::fromStdString(op), this);
				layout->addWidget(radio, row, i % colNum);

				i++;
			}

			auto radios = box->findChildren<QRadioButton*>();
			if (!radios.isEmpty())
			{
				radios.first()->click();
			}
		}
		else if (inputType == "checkBox")
		{
			int i = 0;
			int row = 0;
			auto _j = var["values"];
			int rowMax = lineNum;
			int colNum = std::ceil(_j.size()/(float)rowMax);
			for (auto&& op : _j)
			{
				int row = i / colNum;
				auto radio = new QCheckBox(QString::fromStdString(op), this);
				layout->addWidget(radio, row, i % colNum);

				i++;
			}

			auto radios = box->findChildren<QCheckBox*>();
			if (!radios.isEmpty())
			{
				radios.first()->click();
			}
		}
		else if (inputType == "textEdit")
		{
			nlohmann::json txt="";
			JsonHelper::TryGetValue(var, "values", txt);

			auto textEditor = new QTextEdit(QString::fromStdString(txt.get<std::string>()), this);
			layout->addWidget(textEditor);
			box->setMinimumHeight(60 + 20 * lineNum);

		}

		ContentLayout->addWidget(box);
	}
}

void LabelDetailView::SetModel(QStandardItemModel* model)
{
	ui->tableView->setModel(model);
	ui->tableView->verticalHeader()->hide();
	ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

nlohmann::json LabelDetailView::GetCurDetailInfo()
{
	nlohmann::json j;

	auto model = gModelMgr->GetModel<LabelDetailModel>();
	//auto headers = model->GetTableHeader();
	auto childs = ui->scrollAreaWidgetContents->findChildren<QGroupBox*>();
	qDebug() << childs;

	for (auto&& var : childs)
	{
		auto items = var->findChildren<QRadioButton*>();
		auto title = var->title().toStdString();
		for (auto&& radio : items)
		{
			if (radio && radio->isChecked())
			{
				j.emplace(title, radio->text().toStdString());
				break;
			}
		}

		auto items1 = var->findChildren<QCheckBox*>();

		if (!items1.isEmpty())
		{
			std::stringstream ss;
			for (auto&& radio1 : items1)
			{
				if (radio1 && radio1->isChecked())
				{
					ss << radio1->text().toStdString() << " ";
				}
			}
			auto sss = QString::fromStdString(ss.str()).trimmed();
			j.emplace(title, sss.toStdString());
		}

		auto items2 = var->findChildren<QTextEdit*>();

		for (auto&& edit : items2)
		{
			j.emplace(title, edit->toPlainText().toStdString());
			break;
		}
	}

	return j;
}

void LabelDetailView::on_addButton_clicked()
{
	auto curLabel = gMainWindow->GetCurLabelTypeItem();
	auto model = gModelMgr->GetModel<LabelDetailModel>();
	model->Add(curLabel);

}

void LabelDetailView::on_removeButton_clicked()
{
	auto idx = ui->tableView->currentIndex();
	if (idx.isValid())
	{
		auto model = gModelMgr->GetModel<LabelDetailModel>();
		auto id = model->GetModel()->item(idx.row(), 0);

		model->Remove(id->text().toStdString());
	}
}

void LabelDetailView::on_importButton_clicked()
{
	m_ConfigFilePath = QFileDialog::getOpenFileName(this,
		tr("Import ..."), m_ConfigFilePath, "*.json *.conf");

	if (!m_ConfigFilePath.isEmpty())
	{

		auto j = Helper::LoadJson(m_ConfigFilePath);
		auto model = gModelMgr->GetModel<LabelDetailModel>();
		model->ImportJson(j);
	}
}

void LabelDetailView::on_exportButton_clicked()
{
	//if (s_ExportPath.isEmpty())
	{
		s_ExportPath = QFileDialog::getExistingDirectory(this,
			tr("Export ..."), s_ExportPath);
	}

	if (!s_ExportPath.isEmpty())
	{
		nlohmann::json j;
		auto model = gModelMgr->GetModel<LabelDetailModel>();
		model->ExportJson(j);

		Helper::SaveJson(s_ExportPath, "LabelDetail", j);
	}
}

void LabelDetailView::on_applyButton_clicked()
{
	auto idx = ui->tableView->currentIndex();
	if (!idx.isValid()) return;

	auto model = gModelMgr->GetModel<LabelDetailModel>();
	auto item = model->GetModel()->item(idx.row(), 0);
	if (item == nullptr) return;

	auto id = item->text();

	auto j = GetCurDetailInfo();
	model->Apply(id, j);
}

void LabelDetailView::OnSelectTable(const QModelIndex& idx)
{
	if (!idx.isValid()) return;

	auto model = gModelMgr->GetModel<LabelDetailModel>();
	auto item = model->GetModel()->item(idx.row(), 0);

	if (item == nullptr) return;

	auto id = item->text().toStdString();
	auto headers = model->GetTableHeader();
	auto headerJson = model->GetLabelDetailOptionMap();
	auto tableJson = model->GetLabelDetailTable();
	auto tableLineJson = tableJson[id];


	auto childs = ui->scrollAreaWidgetContents->findChildren<QGroupBox*>();

	for (auto&& girdBox : childs)
	{
		auto title = girdBox->title().toStdString();
		auto j_item = headerJson[title];
		std::string inputType = j_item["inputType"];
		auto value = QString::fromStdString((std::string)tableLineJson[title]);

		if (inputType == "radio")
		{
			auto radios = girdBox->findChildren<QRadioButton*>();
			for (auto&& radio : radios)
			{
				if (radio->text() == value)
				{
					radio->setChecked(true);
				}
			}

		}
		else if (inputType == "checkBox")
		{
			auto values = value.split(" ");
			auto radios = girdBox->findChildren<QCheckBox*>();
			for (auto&& radio : radios)
			{
				if (values.contains(radio->text()))
				{
					radio->setChecked(true);
				}
				else
				{
					radio->setChecked(false);
				}
			}
		}
		else if (inputType == "textEdit")
		{
			auto textEdit = girdBox->findChildren<QTextEdit*>();
			if (!textEdit.isEmpty())
			{
				textEdit.first()->setPlainText(value);
			}
		}
	}
}
