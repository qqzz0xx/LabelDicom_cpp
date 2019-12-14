#pragma once
#include "Header.h"
#include "Types.h"
#include "opencv2/opencv.hpp"

#include <QString>

class LoadMgr
{
public:
	LoadMgr();
	bool LoadFileAsync(const QString& fullPath, bool decryption = false);
	bool LoadFile(const QString& fullPath, bool decryption = false);
	bool LoadMaskData(const QString& fullPath, bool decryption = false);

	vtkImageData* GetImageData(); 
	ImageFileType GetImageFileType() { return m_ImageFileType; }
	vtkImageData* GetFrameByIndex(int index);
	vtkImageData* GetMaskDataByIndex(int index);
	vtkImageData* GetCurMaskData();
	std::map<QString, QString>& GetDicomTags() { return m_DicomTags; }

	const int* GetDims() { return m_Dims; }
	void SaveMaskData(const QString& path);

	int GetMaxFrameCount() { return m_MaxFrameCount; }

	bool IsMainImageLoaded() { return !m_ImageDataList.empty(); }

	static ImageFileType CheckImageType(const QString& suffix);
private:
	void ReadVideo(const QString& fullPath);
	void ReadSingleFrame(const QString& fullPath);
	void ReadVolumeDicom(const QString& fullPath);

	void CVMatToVtkImageData(const cv::Mat& mat, bool flipOverXAxis, vtkImageData* imageData);

private:
	//vtkSmartPointer<vtkImageData> m_ImageData;
	QString m_FileName;
	QString m_CompleteSuffix;
	QString m_AbsoluteFilePath;
	
	ImageFileType m_ImageFileType;

	cv::VideoCapture m_VideoCapture;

	std::vector<vtkSmartPointer<vtkImageData>> m_ImageDataList;
	std::map<int, vtkSmartPointer<vtkImageData>> m_MaskImageDataList;

	int m_MaxFrameCount;
	int m_CurFrameCount = 0;
	int m_Dims[3] = {};
	double m_OriginalSpacing[3] = {};

	std::map<QString, QString> m_DicomTags;
};

