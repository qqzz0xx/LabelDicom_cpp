#pragma once
class QIcon;
class QBrush;
class QColor;
class QPixmap;

namespace QtHelper
{
	QPixmap CreateColorBoxIcon(int w, int h, const QBrush &brush);
	void QColorToVTKRGB(const QColor& color, double* rgb);
	void QColorToVTKRGBA(const QColor& color, double* rgb);
};

