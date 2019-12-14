#include "CodingHelper.h"
#include "qaesencryption.h"
#include "GlobalVal.h"

#include <QCryptographicHash>

void CodingHelper::DeCoding(vtkImageData* data)
{
	auto dim = data->GetDimensions();
	auto type = data->GetScalarType();
	if (type != VTK_UNSIGNED_SHORT) return;

	auto ptr = (uint16*)data->GetScalarPointer();

	for (size_t i = 0; i < dim[0]; i++)
	{
		for (size_t j = 0; j < dim[1]; j++)
		{
			for (size_t k = 0; k < dim[2]; k++)
			{
				uint16 d = (6 * (i + j + k) * (i * k * j)) % 32000;
				ptr[i * dim[1] * dim[2] + j * dim[2] + k] -= d;
			}
		}
	}
}

void CodingHelper::EnCoding(vtkImageData* data)
{
	auto dim = data->GetDimensions();
	auto type = data->GetScalarType();
	if (type != VTK_UNSIGNED_SHORT) return;

	auto ptr = (uint16*)data->GetScalarPointer();

	for (size_t i = 0; i < dim[0]; i++)
	{
		for (size_t j = 0; j < dim[1]; j++)
		{
			for (size_t k = 0; k < dim[2]; k++)
			{
				uint16 d = (6 * (i + j + k) * (i * k * j)) % 32000;
				ptr[i * dim[1] * dim[2] + j * dim[2] + k] += d;
			}
		}
	}
}

void CodingHelper::DeCodingJson(QByteArray& bytes)
{
	bytes = QByteArray::fromBase64(bytes);
}

void CodingHelper::EnCodingJson(QByteArray& bytes)
{
	bytes = bytes.toBase64();
}

QString CodingHelper::DeCodingAES(const QFileInfo& info, QString root)
{
	auto path_str = root.isEmpty() ? info.absolutePath().toStdString() : root.toStdString();
	auto base_name = info.baseName().toStdString();
	auto suffix = info.completeSuffix().toStdString();

	std::stringstream ss;
	ss << path_str << "/" << base_name << "_decrypt." << suffix;

	QFile f(info.absoluteFilePath());
	f.open(QFile::ReadOnly);

	QByteArray data = f.readAll();
	QByteArray decodeText = DecodeBytes(data);


	QString outPath = QString::fromStdString(ss.str());
	QFile outfile(outPath);
	outfile.open(QFile::WriteOnly | QFile::Truncate);
	outfile.write(decodeText);
	outfile.flush();
	outfile.close();

	return outPath;
}

void CodingHelper::EnCodingAES(const QFileInfo& info)
{
	QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::ECB);

	auto path_str = info.absolutePath().toStdString();
	auto base_name = info.baseName().toStdString();
	auto suffix = info.completeSuffix().toStdString();

	std::stringstream ss;
	ss << path_str << "/" << base_name << "_encrypt." << suffix;

	QFile f(info.absoluteFilePath());
	f.open(QFile::ReadOnly);

	QByteArray data = f.readAll();
	QByteArray encodeText = EncodeBytes(data);

	QFile outfile(QString::fromStdString(ss.str()));
	outfile.open(QFile::WriteOnly | QFile::Truncate);
	outfile.write(encodeText);
	outfile.flush();
	outfile.close();
}

QByteArray CodingHelper::EncodeBytes(const QByteArray& bytes)
{
	QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::ECB);
	auto key = g_ConfigVal.AES_KEY;
	QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
	std::cout << std::string("hash key: ") << hashKey.toBase64().toStdString() << std::endl;
	QByteArray encodeText = encryption.encode(bytes, hashKey);
	return encodeText;
}

QByteArray CodingHelper::DecodeBytes(const QByteArray& bytes)
{
	auto key = g_ConfigVal.AES_KEY;
	QByteArray hashKey = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Sha256);
	QAESEncryption encryption(QAESEncryption::AES_256, QAESEncryption::ECB);
	QByteArray decodeText = encryption.decode(bytes, hashKey);
	decodeText = encryption.removePadding(decodeText);
	return decodeText;
}
