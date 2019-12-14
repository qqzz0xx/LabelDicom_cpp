#pragma once
#include "Header.h"
#include "ui_LabelDetailView.h"
#include "JsonHelper.h"

class LabelDetailView : public QWidget
{
	Q_OBJECT
public:
	explicit LabelDetailView(QWidget* parent = Q_NULLPTR);
	
	void LoadOption(const nlohmann::json& j);
	void SetModel(QStandardItemModel* model);
	nlohmann::json GetCurDetailInfo();

	Ui_LabelDetailView* ui;

private slots:
	void on_addButton_clicked();
	void on_removeButton_clicked();
	void on_importButton_clicked();
	void on_exportButton_clicked();
	void on_applyButton_clicked();
	void OnSelectTable(const QModelIndex& idx);

private:

	QString m_ConfigFilePath;
};

