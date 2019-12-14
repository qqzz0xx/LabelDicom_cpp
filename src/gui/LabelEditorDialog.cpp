#include "LabelEditorDialog.h"
#include "ModelManager.h"
#include "AppMain.h"
#include "ColorLabelTableModel.h"
#include "QtHelper.h"
#include "Helper.h"
#include <QDebug>
#include <QFileDialog>

LabelEditorDialog::LabelEditorDialog(QWidget * parent, Qt::WindowFlags f):
	QDialog(parent),
	ui(new Ui_LabelEditorDialog)
{
	ui->setupUi(this);

	//auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	//auto func = [&](auto color, auto label)
	//{
	//	auto it = model->AddColorLabel(color, label);
	//	m_CurLabelItem = it;
	//};

	//func(QColor(255, 0, 0), tr("Label1"));
	//func(QColor(255, 255, 0), tr("Label2"));
	//func(QColor(0, 0, 255), tr("Label3"));


	connect(ui->inLabelDescription, &QLineEdit::textEdited, this, [&](auto txt) {
		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		m_CurLabelItem->Desc = txt;
		model->UpdateColorLabel(m_CurLabelItem);
		emit LabelTypeChanged(m_CurLabelItem.get());
	});

	connect(ui->inLabelDescription, &QLineEdit::textChanged, this, [&](auto txt) {
		if (m_CurLabelItem == nullptr) return;
		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		m_CurLabelItem->Desc = txt;
		model->UpdateColorLabel(m_CurLabelItem);
		emit LabelTypeChanged(m_CurLabelItem.get());
	});

	connect(ui->btnLabelColor, &QColorButtonWidget::valueChanged, this, [&]() {
		if (m_CurLabelItem == nullptr) return;
		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		auto color = ui->btnLabelColor->value();
		double a = m_CurLabelItem->Color.alpha();
		m_CurLabelItem->Color.setRgb(color.rgb());
		m_CurLabelItem->Color.setAlpha(a);

		model->UpdateColorLabel(m_CurLabelItem);
		emit LabelTypeChanged(m_CurLabelItem.get());
	});

	connect(ui->inLabelOpacitySlider, &QSlider::valueChanged, this, [&](auto v) {
		if (m_CurLabelItem == nullptr) return;
		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		m_CurLabelItem->Color.setAlpha(v);
		model->UpdateColorLabel(m_CurLabelItem);
		emit LabelTypeChanged(m_CurLabelItem.get());
	});

	connect(ui->treeView, &QTreeView::clicked, this, &LabelEditorDialog::OnActivated);
}

LabelEditorDialog::~LabelEditorDialog()
{
}

void LabelEditorDialog::SetModel(QStandardItemModel * model)
{
	m_Model = model;
	ui->treeView->setModel(model);
	//ui->tableView->horizontalHeader()->setStretchLastSection(true);
	ui->treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	ui->treeView->header()->setSectionResizeMode(2, QHeaderView::Stretch);

	ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->treeView->setProperty("SH_ItemView_ActivateItemOnSingleClick", true);
}

void LabelEditorDialog::UpdateRightInfo(std::shared_ptr<ColorLabelItem>& it)
{
	if (m_CurLabelItem && it && it->ID == m_CurLabelItem->ID)
	{
		ui->inLabelDescription->blockSignals(true);
		ui->inLabelDescription->setText(it->Desc);
		ui->inLabelDescription->blockSignals(false);
		ui->btnLabelColor->blockSignals(true);
		ui->btnLabelColor->setValue(it->Color);
		ui->btnLabelColor->blockSignals(false);
		ui->inLabelOpacitySlider->blockSignals(true);
		ui->inLabelOpacitySlider->setValue(it->Color.alpha());
		ui->inLabelOpacitySlider->blockSignals(false);
		ui->inLabelOpacitySpinner->blockSignals(true);
		ui->inLabelOpacitySpinner->setValue(it->Color.alpha());
		ui->inLabelOpacitySpinner->blockSignals(false);
	}
}

void LabelEditorDialog::on_delBtn_clicked()
{
	auto idx = ui->treeView->currentIndex();
	if (idx.isValid())
	{
		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		auto id = m_Model->item(idx.row(), 0)->text();
		model->RemoveColorLabel(id);
	}
}

void LabelEditorDialog::on_exportBtn_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"));

	if (path != nullptr)
	{
		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		nlohmann::json j;
		model->ExportJson(j);

		Helper::SaveJson(path, "ColorTable", j);
	}
}

void LabelEditorDialog::on_importBtn_clicked()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Open Directory"));

	if (path != nullptr)
	{
		auto j = Helper::LoadJson(path);
		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		model->ImportJson(j);
	}
}

void LabelEditorDialog::OnActivated(const QModelIndex & idx)
{
	qDebug() << idx;

	auto id = m_Model->item(idx.row(), 0)->text();
	auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	auto it = model->GetColorLabel(id);
	m_CurLabelItem = it;

	UpdateRightInfo(it);

	qDebug() << "on_treeView_activated" <<it->Color;
}

void LabelEditorDialog::on_newBtn_clicked()
{
	auto model = gModelMgr->GetModel<ColorLabelTableModel>();
	auto it =  model->AddColorLabel(ui->btnLabelColor->value(), tr("Label"));
	m_CurLabelItem = it;

	UpdateRightInfo(it);
}
