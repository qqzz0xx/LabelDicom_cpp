#pragma once
#include "Header.h"
#include "json.hpp"
#include "opencv2/opencv.hpp"
#include "Types.h"
#include <QImage>

class vtkTextProperty;

namespace Helper
{
	nlohmann::json LoadJson(const QString& path, bool cryption = false);
	void SaveJson(const QString& path, const std::string& type, const nlohmann::json& j, bool cryption = false);

	void CVMatToVtkImageData(const cv::Mat& mat, bool flipOverXAxis, vtkImageData* imageData);
	
	void VtkImageDataToCVMat(vtkImageData* imageData, cv::Mat& mat);
	
	QImage mat2qim(cv::Mat& mat);

	cv::Mat qim2mat(QImage& qim);

	void SetVtkFont(vtkTextProperty* prop);

	ImageShowType FileTypeToShowType(ImageFileType type);

	void NormalizBuffer(uint8* buf, size_t len);
}



