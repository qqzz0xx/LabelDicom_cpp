#pragma once
#include "ModelBase.h"
#include <QString>

class NSliceWidget;
class MprPositionModel : public ModelBase
{
public:
	MprPositionModel();
	~MprPositionModel();

	double m_SceenPos[3] = {};
	double m_WorldPos[3] = {};
	double m_ImageXY[3] = {};
	double m_VoxPos[3] = {};
	int m_Pixel = 0;

	QString ToQtString();
};

