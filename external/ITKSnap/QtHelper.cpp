#include "QtHelper.h"


#include <QRect>
#include <QPixmap>
#include <QColor>
#include <QPainter>
#include <QIcon>

QPixmap QtHelper::CreateColorBoxIcon(int w, int h, const QBrush & brush)
{
	QRect r(2, 2, w - 5, w - 5);
	QPixmap pix(w, h);
	pix.fill(QColor(0, 0, 0, 0));
	QPainter paint(&pix);
	paint.setPen(Qt::black);
	paint.setBrush(brush);
	paint.drawRect(r);
	return pix;
}

void QtHelper::QColorToVTKRGB(const QColor& color, double* rgb)
{
	rgb[0] = color.red() / 255.0;
	rgb[1] = color.green() / 255.0;
	rgb[2] = color.blue() / 255.0;
}

void QtHelper::QColorToVTKRGBA(const QColor& color, double* rgb)
{
	rgb[0] = color.red() / 255.0;
	rgb[1] = color.green() / 255.0;
	rgb[2] = color.blue() / 255.0;
	rgb[3] = color.alpha() / 255.0;
}
