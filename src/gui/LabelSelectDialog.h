#pragma once

#include "Header.h"
#include "ColorLabelTableModel.h"

#include <QDialog>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>

class LabelSelectDialog : public QDialog
{
public:
	explicit LabelSelectDialog(QWidget *parent = Q_NULLPTR);

	~LabelSelectDialog();

	void SetModel(QSortFilterProxyModel* model);
	const std::shared_ptr<ColorLabelItem>& value() { return m_Label; }

	std::function<void(int id)> SelectCallback = nullptr;
private:
	QTableView* m_TableView;
	QSortFilterProxyModel* m_Model;
	std::shared_ptr<ColorLabelItem> m_Label;
};

