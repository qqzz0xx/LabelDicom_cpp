#include "LabelSelectDialog.h"
#include "ColorLabelTableModel.h"
#include "AppMain.h"
#include <QVBoxLayout>
#include <QTableView>


LabelSelectDialog::LabelSelectDialog(QWidget *parent) :
	QDialog(parent)
{
	this->setWindowTitle("LabelSelectDialog");
	this->setLayout(new QVBoxLayout);
	m_TableView = new QTableView;
	this->layout()->addWidget(m_TableView);
	this->resize(300, 300);

	connect(m_TableView, &QTableView::activated, [&](const QModelIndex &index) {
		auto m = (QStandardItemModel*)m_Model->sourceModel();
		auto item = m->item(index.row(), 0);
		auto id = item->text();

		auto model = gModelMgr->GetModel<ColorLabelTableModel>();
		m_Label = model->GetColorLabel(id);

		accept();
	});
}

LabelSelectDialog::~LabelSelectDialog()
{
}

void LabelSelectDialog::SetModel(QSortFilterProxyModel* model)
{
	m_Model = model;
	m_TableView->setModel(model);
	m_TableView->verticalHeader()->hide();
	m_TableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_TableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	m_TableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

	m_TableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_TableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
