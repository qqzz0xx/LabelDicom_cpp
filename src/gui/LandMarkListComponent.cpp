#include "LandMarkListComponent.h"


LandMarkListComponent::LandMarkListComponent(QWidget * parent, Qt::WindowFlags f):
	QWidget(parent),
	ui(new Ui_LandMarkListComponent)
{
	ui->setupUi(this);
}

LandMarkListComponent::~LandMarkListComponent()
{
}
