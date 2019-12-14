#pragma once

#include "ui_openfiledialog.h"

#include <QDialog>

class OpenFileDialog : public QDialog
{
	Q_OBJECT
public:
	explicit OpenFileDialog(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~OpenFileDialog();

	QString GetFileName();
	void SetOpenDir(bool v) { m_IsOpenDir = v; }

	bool IsEncryption();
	static void SetFileName(QString&& name);
	int GetFileType(); // 0-Image 1-Video 2-Volume


private slots:
	void on_browse_clicked();
public:
	Ui_OpenFileDialog* ui;
	QString FilterString;
	bool m_IsOpenDir = false;
};

