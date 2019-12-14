#pragma once
#include "Header.h"
#include "ui_LabelEditorDialog.h"
#include <QDialog>
#include <QStandardItemModel>

struct ColorLabelItem;
class LabelEditorDialog : public QDialog
{
	Q_OBJECT
public:
	explicit LabelEditorDialog(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~LabelEditorDialog();
	void SetModel(QStandardItemModel* model);
	void UpdateRightInfo(std::shared_ptr<ColorLabelItem>& label);
	Ui_LabelEditorDialog* ui;

signals:
	void LabelTypeChanged(ColorLabelItem* labelType);

public slots:
	void on_newBtn_clicked();
	void on_delBtn_clicked();
	void on_exportBtn_clicked();
	void on_importBtn_clicked();

	void OnActivated(const QModelIndex& idx);

private:
	QStandardItemModel* m_Model;
	int m_Index = 0;
	std::shared_ptr<ColorLabelItem> m_CurLabelItem;
};

