#include "LoadDroppedFileDialog.h"

#include "QHBoxLayout"
#include "QPushButton"
#include "QCheckBox"
#include "QVBoxLayout"

LoadDroppedFileDialog::LoadDroppedFileDialog(QWidget* parent): QDialog(parent)
{
	auto layout = new QVBoxLayout(this);
	layout->setSpacing(10);
	this->resize(200, 160);

	auto btn1 = new QPushButton("MainImage");
	auto btn2 = new QPushButton("LabelData");
	auto check1 = new QCheckBox("encryption");

	layout->addWidget(btn1);
	layout->addWidget(btn2);
	layout->addWidget(check1);

	m_IsEncryption = check1->isChecked();

	connect(btn1, &QPushButton::clicked, [&] {
		m_IsLoadMainImage = true;
		accept();
	});

	connect(btn2, &QPushButton::clicked, [&] {
		m_IsLoadMainImage = false;
		accept();
	});

	connect(check1, &QCheckBox::clicked, [&, check1] {
		m_IsEncryption = check1->isChecked();
	});
}
