#include "LabelListComponent.h"
#include "LandMarkListModel.h"
#include "BoundingBox3DLabelModel.h"
#include "AppMain.h"
#include "Helper.h"
#include "LoadMgr.h"

#include <QFileDialog>

LabelListComponent::LabelListComponent(QWidget * parent, Qt::WindowFlags f) :
	QWidget(parent),
	ui(new Ui_LabelListComponent)
{
	ui->setupUi(this);

	connect(ui->treeView_2, &QTreeView::clicked, this, &LabelListComponent::OnBoxItemClicked);

}

LabelListComponent::~LabelListComponent()
{
}

void LabelListComponent::SetLandMarkModel(QStandardItemModel * model)
{
	m_Model = model;
	auto view = ui->treeView;
	view->setModel(model);
	view->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	view->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	view->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	view->header()->setSectionResizeMode(3, QHeaderView::Stretch);

	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	view->setProperty("SH_ItemView_ActivateItemOnSingleClick", true);
}

void LabelListComponent::SetBoundingBoxModel(QStandardItemModel* model)
{
	m_BoxModel = model;
	auto view = ui->treeView_2;
	view->setModel(model);
	view->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	view->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	view->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	view->header()->setSectionResizeMode(3, QHeaderView::Stretch);

	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setEditTriggers(QAbstractItemView::NoEditTriggers);
	view->setProperty("SH_ItemView_ActivateItemOnSingleClick", true);
}

void LabelListComponent::OnBoxItemClicked(const QModelIndex& idx)
{
	if (!idx.isValid()) return;
	
	auto item = (QStandardItemUUID*)m_BoxModel->item(idx.row(), 3);
	auto id = item->uuid;

	auto model = gModelMgr->GetModel<BoundingBox3DLabelModel>();
	model->SetBoundingBoxActorEnable(id);

	gMainWindow->RenderAll();
}

QString LabelListComponent::GetSelectedBoxID()
{
	auto idx =  ui->treeView_2->currentIndex();
	if (idx.isValid())
	{
		auto item = (QStandardItemUUID*)m_BoxModel->item(idx.row(), 3);
		auto id = item->uuid;

		return QString::fromStdString(id);
	}
		
	return QString();
}

void LabelListComponent::on_exportBtn_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"));

	if (path != nullptr)
	{
		auto model = gModelMgr->GetModel<LandMarkListModel>();
		model->SaveToJson(path);
		gLoadMgr->SaveMaskData(path);
	}
}

void LabelListComponent::on_removeButton_clicked()
{
	auto idx = ui->treeView->currentIndex();
	if (!idx.isValid()) return;
	auto item = (QStandardItemUUID*)m_Model->item(idx.row(), 3);

	auto model = gModelMgr->GetModel<LandMarkListModel>();
	model->Remove(QString::fromStdString(item->uuid));
}

void LabelListComponent::on_exportBoxBtn_clicked()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"));

	if (path != nullptr)
	{
		auto model = gModelMgr->GetModel<BoundingBox3DLabelModel>();
		nlohmann::json j;
		model->ExportJson(j);
		Helper::SaveJson(path, "BoundingBox3D", j);
	}
}

void LabelListComponent::on_removeBoxBtn_clicked()
{
	auto idx = ui->treeView_2->currentIndex();
	if (!idx.isValid()) return;

	//auto id = m_BoxModel->item(idx.row(), 3)->text();
	//auto model = gModelMgr->GetModel<BoundingBox3DLabelModel>();
	//model->Remove(id);
	auto item = (QStandardItemUUID*)m_BoxModel->item(idx.row(), 3);
	auto model = gModelMgr->GetModel<BoundingBox3DLabelModel>();
	model->Remove(QString::fromStdString(item->uuid));
}

