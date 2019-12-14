#include <cstdio>
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "vtkImageShiftScale.h"
#include "vtkNIFTIImageWriter.h"

#include "CodingHelper.h"
#include "GlobalVal.h"


#include "QFileInfo"
#include "QDir"
#include "QDataStream"
#include "qaesencryption.h"
#include "QCryptographicHash"

void EncodeJson(const QFileInfo& info)
{
	QFile f(info.absoluteFilePath());
	f.open(QFile::ReadOnly | QFile::Text);

	QByteArray bytes = f.readAll();
	CodingHelper::EnCodingJson(bytes);
	f.close();

	auto path_str = info.absolutePath().toStdString();
	auto base_name = info.baseName().toStdString();

	std::stringstream ss;
	ss << path_str << "/" << base_name << "_encrypt" << ".json";
	QFile outfile(QString::fromStdString(ss.str()));
	outfile.open(QFile::WriteOnly | QFile::Truncate);
	outfile.write(bytes);
	outfile.flush();
	outfile.close();

}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << std::string("usage: Encrypt [FILE_PATH]") << std::endl;
		return 0;
	}

	auto info = QFileInfo(argv[1]);
	auto suffix = info.completeSuffix();
	if (suffix == "json")
	{
		EncodeJson(info);
	}
	else if (suffix == "mp4" || suffix == "avi")
	{
		CodingHelper::EnCodingAES(info);
	}
	else
	{

		auto path_str = info.absolutePath().toStdString();
		auto base_name = info.baseName().toStdString();


		using ImageType = itk::Image<uint8, 3>;
		auto reader = itk::ImageFileReader< ImageType >::New();
		reader->SetFileName(info.absoluteFilePath().toStdString().c_str());

		auto filter = itk::ImageToVTKImageFilter< ImageType >::New();
		filter->SetInput(reader->GetOutput());
		filter->Update();

		//auto imageData = vtkSmartPointer<vtkImageData>::New();
		//imageData->DeepCopy(filter->GetOutput());

		auto shiftScale = vtkSmartPointer<vtkImageShiftScale>::New();
		shiftScale->SetInputData(filter->GetOutput());
		shiftScale->SetOutputScalarTypeToUnsignedShort();
		shiftScale->Update();

		auto img = shiftScale->GetOutput();

		CodingHelper::EnCoding(img);


		auto writer = vtkSmartPointer<vtkNIFTIImageWriter>::New();
		std::stringstream ss;
		ss << path_str << "/" << base_name << "_encrypt" << ".nii.gz";
		writer->SetFileName(ss.str().c_str());
		writer->SetInputData(img);
		writer->Update();

	}

	std::cout << std::string("encode: ") << info.absoluteFilePath().toStdString() << std::endl;

	return 0;
}