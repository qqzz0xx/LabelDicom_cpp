#pragma once
#include "Header.h"
#include <QDialog>

class LoadDroppedFileDialog : public QDialog
{
public:
	explicit LoadDroppedFileDialog(QWidget* parent = Q_NULLPTR);

	bool IsLoadMainImage() { return m_IsLoadMainImage; }
	bool IsEncryption() { return m_IsEncryption; }
private:
	bool m_IsLoadMainImage;
	bool m_IsEncryption;
};