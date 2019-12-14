#include "MprPositionModel.h"
#include "CoordHelper.h"


MprPositionModel::MprPositionModel()
{
	m_Name = typeid(this).name();
}


MprPositionModel::~MprPositionModel()
{
}


QString MprPositionModel::ToQtString()
{
	auto str = QStringLiteral("��Ļ���� : (%1,%2) || �������� : (%3,%4,%5) || ͼ������ : (%6,%7) || ������ : (%8,%9,%10) || Pixel : (%11)").
		arg(m_SceenPos[0])
		.arg(m_SceenPos[1])
		.arg(m_WorldPos[0])
		.arg(m_WorldPos[1])
		.arg(m_WorldPos[2])
		.arg(m_ImageXY[0])
		.arg(m_ImageXY[1])
		.arg(m_VoxPos[0])
		.arg(m_VoxPos[1])
		.arg(m_VoxPos[2])
		.arg(m_Pixel);
	return str;
}
