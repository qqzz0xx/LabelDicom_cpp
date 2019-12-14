#pragma once
#include "Header.h"
#include "QFileInfo"

namespace CodingHelper
{
	void DeCoding(vtkImageData* data);
	void EnCoding(vtkImageData* data);
	void DeCodingJson(QByteArray& bytes);
	void EnCodingJson(QByteArray& bytes);
	QString DeCodingAES(const QFileInfo& info, QString root = QString());
	void EnCodingAES(const QFileInfo& info);
	QByteArray EncodeBytes(const QByteArray& bytes);
	QByteArray DecodeBytes(const QByteArray& bytes);
};

