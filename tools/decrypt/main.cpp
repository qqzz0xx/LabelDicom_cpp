#include <cstdio>
#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "vtkImageShiftScale.h"
#include "vtkNIFTIImageWriter.h"

#include "Header.h"
#include "CodingHelper.h"
#include "GlobalVal.h"

#include "QFileInfo"
#include "QDir"
#include "QCryptographicHash"
#include "qaesencryption.h"

void DecodeVol(std::string&& path_str, std::string&& base_name, std::string&& full_path)
{
	using ImageType = itk::Image<uint16, 3>;
	auto reader = itk::ImageFileReader< ImageType >::New();
	reader->SetFileName(full_path.c_str());

	auto filter = itk::ImageToVTKImageFilter< ImageType >::New();
	filter->SetInput(reader->GetOutput());
	filter->Update();

	CodingHelper::DeCoding(filter->GetOutput());
	//auto imageData = vtkSmartPointer<vtkImageData>::New();
	//imageData->DeepCopy(filter->GetOutput());

	auto shiftScale = vtkSmartPointer<vtkImageShiftScale>::New();
	shiftScale->SetInputData(filter->GetOutput());
	shiftScale->SetOutputScalarTypeToUnsignedChar();
	shiftScale->Update();


	auto writer = vtkSmartPointer<vtkNIFTIImageWriter>::New();
	std::stringstream ss;
	ss << path_str << "/" << base_name << "_decrypt" << ".nii.gz";
	writer->SetFileName(ss.str().c_str());
	writer->SetInputData(shiftScale->GetOutput());
	writer->Update();

}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << std::string("usage: Decrypt [FILE_PATH]") << std::endl;
		return 0;
	}

	auto info = QFileInfo(argv[1]);
	auto path_str = info.absolutePath().toStdString();
	auto base_name = info.baseName().toStdString();
	auto full_path = info.absoluteFilePath().toStdString();
	auto suffix = info.completeSuffix();

	if (suffix == "avi" || suffix == "mp4" || suffix == "cdat")
	{
		CodingHelper::DeCodingAES(info);
	}
	else
	{
		DecodeVol(std::move(path_str), std::move(base_name), std::move(full_path));
	}
	
	std::cout << std::string("decode: ") << full_path << std::endl;

	return 0;
}