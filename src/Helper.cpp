#include "Helper.h"
#include "LocalConfig.h"
#include "GlobalVal.h"
#include "CodingHelper.h"

#include "qaesencryption.h"
#include "GlobalVal.h"
#include "vtkTextProperty.h"
#include "AppMain.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <QCryptographicHash>
#include <QFile>
#include <QDir>

namespace fs = std::filesystem;
nlohmann::json Helper::LoadJson(const QString& path, bool cryption)
{
	if (!QFile::exists(path)) return false;
	
	nlohmann::json j;
	//std::ifstream ifs(full, std::ios::binary);
	try
	{
		//std::string txt;
		//ifs >> j;

		QFile f(path);
		f.open(QFile::ReadOnly);
		QByteArray bytes = f.readAll();
		f.close();

		if (cryption)
		{
			qDebug() << "file read size: " << bytes.size();

			bytes = CodingHelper::DecodeBytes(bytes);

			qDebug() << "decodeTxt size" << bytes.size();
		}

		auto txt = bytes.toStdString();

		j = nlohmann::json::parse(txt);

	}
	catch (std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
	}

	return j;
}

void Helper::SaveJson(const QString& path, const std::string& type, const nlohmann::json& j, bool cryption)
{
	try
	{
		auto txt = j.dump(4);

		QByteArray bytes;
		bytes.append(QString::fromStdString(txt));

		//if (cryption)
		//{
		//	CodingHelper::EnCodingJson(bytes);
		//}

		QDir dir(path);
		if (!dir.exists())
		{
			dir.mkpath(path);
		}

		std::stringstream ss;
		ss << path.toStdString() << "/" << g_GlobalVal.FileName.toStdString() << "_" << type << ".json";
		QFile outfile(QString::fromStdString(ss.str()));
		outfile.open(QFile::WriteOnly | QFile::Truncate);
		outfile.write(bytes);
		outfile.flush();
		outfile.close();

	}
	catch (std::exception & ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void Helper::CVMatToVtkImageData(const cv::Mat& sourceCVImage, bool flipOverXAxis, vtkImageData* outputVtkImage)
{
	double spacing[3] = { 1, 1, 1 };
	double origin[3] = { 0, 0, 0 };
	int extent[6] = { 0, sourceCVImage.cols - 1, 0, sourceCVImage.rows - 1, 0, 0 };
	auto numOfChannels = sourceCVImage.channels();
	outputVtkImage->SetSpacing(spacing);
	outputVtkImage->SetOrigin(origin);
	outputVtkImage->SetExtent(extent);
	outputVtkImage->SetDimensions(sourceCVImage.cols, sourceCVImage.rows, 1);
	outputVtkImage->AllocateScalars(VTK_UNSIGNED_CHAR, numOfChannels);

	cv::Mat tempCVImage;
	if (flipOverXAxis) { // Normaly you should flip the image!
		cv::flip(sourceCVImage, tempCVImage, 0);
	}
	else {
		tempCVImage = sourceCVImage;
	}

	// the number of byes in the cv::Mat, assuming the data type is uchar
	size_t byte_count = sourceCVImage.cols * sourceCVImage.rows * numOfChannels * sizeof(unsigned char);

	// copy the internal cv::Mat data into the vtkImageData pointer
	memcpy(outputVtkImage->GetScalarPointer(), tempCVImage.data, byte_count);
}

void Helper::VtkImageDataToCVMat(vtkImageData* vtkImage, cv::Mat& mat)
{
	int imageDimensions[3] = { 0, 0, 0 }; // Width, Hight, Depth --> Depth is not equal to number of image channels!
	vtkImage->GetDimensions(imageDimensions);
	int imageWidth = imageDimensions[0];
	int imageHeight = imageDimensions[1];
	int numberOfImageChannels = vtkImage->GetNumberOfScalarComponents();
	int cvType = 0;
	switch (numberOfImageChannels) {
	case 1: cvType = CV_8UC1; break;
	case 3: cvType = CV_8UC3; break;
	case 4: cvType = CV_8UC4; break;
	default: std::cerr << "Check number of vtk image channels!" << std::endl;
	}

	auto resultingCVMat = cv::Mat(imageHeight, imageWidth, cvType);
	// Loop over the vtkImageData contents.
	//for (int heightPos = 0; heightPos < imageHeight; heightPos++) {
	//	for (int widthPos = 0; widthPos < imageWidth; widthPos++) {
	//		auto pixel = static_cast<unsigned char*>(vtkImage->GetScalarPointer(widthPos, heightPos, 0));
	//		resultingCVMat.at<unsigned char>(heightPos, widthPos) = *pixel;
	//	}
	//}

	size_t byte_count = imageWidth * imageHeight * sizeof(unsigned char) * numberOfImageChannels;

	// copy the internal cv::Mat data into the vtkImageData pointer
	memcpy(resultingCVMat.data, vtkImage->GetScalarPointer(), byte_count);

	mat = resultingCVMat;
}

QImage Helper::mat2qim(cv::Mat& mat)
{
	cvtColor(mat, mat, cv::COLOR_BGR2RGB);
	QImage qim((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step,
		QImage::Format_RGB888);
	return qim;
}

cv::Mat Helper::qim2mat(QImage& qim)
{
	cv::Mat mat = cv::Mat(qim.height(), qim.width(),
		CV_8UC3, (void*)qim.constBits(), qim.bytesPerLine());
	return mat;
}

void Helper::SetVtkFont(vtkTextProperty* prop)
{
	prop->SetFontFamily(VTK_FONT_FILE);

	auto path = gAppMain->applicationDirPath() + "/YaHei-Consolas.ttf";

	prop->SetFontFile(path.toStdString().c_str());
}

ImageShowType Helper::FileTypeToShowType(ImageFileType type)
{
	switch (type)
	{
	case ImageFileType_None:
		break;
	case ImageFileType_NiTFI:
	case ImageFileType_MetaImage:
	case ImageFileType_VolumeDicom:
		return ImageShowType_Volume;
		break;
	case ImageFileType_SingleFrame:
		return ImageShowType_Image;
		break;
	case ImageFileType_SequenceFrame:
		return ImageShowType_Video;
		break;
	default:
		break;
	}

	return ImageShowType_Image;
}

void Helper::NormalizBuffer(uint8* buf, size_t len)
{
	auto p = buf;
	std::map<int, int> hist;
	for (size_t i = 0; i < len; i++)
	{
		auto idx = p[i];
		auto it = hist.find(idx);
		if (it == hist.end())
		{
			hist[idx] = 1;
		}
		else
		{
			hist[idx] = hist[idx] + 1;
		}
	}

	int max = (--hist.end())->first;

	auto cmp = [](auto& l, auto& r)
	{
		return l.second > r.second;
	};

	std::set<std::pair<int, int>, decltype(cmp)> m(hist.begin(), hist.end(), cmp);

	uint8 bgVal = m.begin()->first;
	float d = max - bgVal;

	for (size_t i = 0; i < len; i++)
	{
		p[i] = p[i] < bgVal ? 0 : (p[i] - bgVal) / d * 255;
	}
}

