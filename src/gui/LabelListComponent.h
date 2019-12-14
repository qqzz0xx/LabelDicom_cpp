#pragma once
#include "Header.h"
#include "ui_LableListComponent.h"
#include <QWidget>

class LabelListComponent : public QWidget
{
	Q_OBJECT
public:
	explicit LabelListComponent(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~LabelListComponent();
	Ui_LabelListComponent* ui;

	void SetLandMarkModel(QStandardItemModel* model);
	void SetBoundingBoxModel(QStandardItemModel* model);

	void OnBoxItemClicked(const QModelIndex& idx);
	QString GetSelectedBoxID();


private slots:
	void on_exportBtn_clicked();
	void on_removeButton_clicked();

	void on_exportBoxBtn_clicked();
	void on_removeBoxBtn_clicked();


private:
	QStandardItemModel* m_Model;
	QStandardItemModel* m_BoxModel;
};

