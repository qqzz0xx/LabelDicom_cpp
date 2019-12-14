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
	auto str = QStringLiteral("屏幕坐标 : (%1,%2) || 世界坐标 : (%3,%4,%5) || 图像坐标 : (%6,%7) || 体坐标 : (%8,%9,%10) || Pixel : (%11)").
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
