#pragma once
#include "ui_LandMarkListComponent.h"
#include <QWidget>

class LandMarkListComponent :
	public QWidget
{
	Q_OBJECT
public:
	explicit LandMarkListComponent(QWidget* parent = Q_NULLPTR, 
		Qt::WindowFlags f = Qt::WindowFlags());
	~LandMarkListComponent();

	Ui_LandMarkListComponent* ui;

};

