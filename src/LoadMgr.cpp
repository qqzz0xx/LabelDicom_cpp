#include "LoadMgr.h"
#include "GlobalVal.h"
#include "CodingHelper.h"

#include "vtkNIFTIImageWriter.h"

#include "vtkMetaImageReader.h"
#include "vtkNIFTIImageReader.h"
#include "vtkDICOMImageReader.h"
#include "vtkBMPReader.h"
#include "vtkJPEGReader.h"
#include "vtkPNGReader.h"
#include "vtkAVSucdReader.h"
#include "vtkImageChangeInformation.h"
#include "vtkImageShiftScale.h"
#include "vtkImageReslice.h"
#include "vtkzlib/zlib.h"

#include "itkImageFileReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"
#include "ResliceDelegate.h"


#include "opencv2/opencv.hpp"

#include <QTemporaryFile>
#include <QFileInfo>
#include <chrono>
#include "NThreadPool.h"

using namespace cv;

LoadMgr::LoadMgr()
{
}

bool LoadMgr::LoadFileAsync(const QString& fullPath, bool decryption /*= false*/)
{
	auto t = new NThread;
	t->Callback = [&]() {
		LoadFile(fullPath, decryption);
	};
	t->start();
	return false;
}

bool LoadMgr::LoadFile(const QString& fullPath, bool decryption)
{
	m_ImageDataList.clear();
	m_MaskImageDataList.clear();
	m_VideoCapture.release();
	m_CurFrameCount = 0;
	m_MaxFrameCount = 1;

	auto info = QFileInfo(fullPath);
	m_FileName = info.baseName();
	m_AbsoluteFilePath = info.absoluteFilePath();
	m_CompleteSuffix = info.completeSuffix();
	m_ImageFileType = CheckImageType(m_CompleteSuffix);

	if (m_ImageFileType == ImageFileType_None) return false;

	g_GlobalVal.FileName = info.fileName().replace('.', '_');
	g_GlobalVal.IsPrjectChanged = false;

	LOG(INFO) << "suffix:" << m_CompleteSuffix.toStdString();
	LOG(INFO) << "m_FileName:" << info.baseName().toStdString();
	LOG(INFO) << "completeSuffix:" << info.completeSuffix().toStdString();
	LOG(INFO) << "completeBaseName:" << info.completeBaseName().toStdString();
	LOG(INFO) << "absoluteFilePath:" << info.absoluteFilePath().toStdString();

	auto func = [&](auto reader)
	{
		reader->SetFileName(fullPath.toLocal8Bit().toStdString().c_str());
		reader->Update();

		if (decryption)
		{
			CodingHelper::DeCoding(reader->GetOutput());
		}

		auto change = vtkSmartPointer<vtkImageChangeInformation>::New();
		change->SetInputData(reader->GetOutput());
		change->SetOutputSpacing(1, 1, 1);
		change->Update();

		auto shiftScale = vtkSmartPointer<vtkImageShiftScale>::New();
		shiftScale->SetInputData(change->GetOutput());
		shiftScale->SetOutputScalarTypeToUnsignedChar();
		shiftScale->Update();

		auto imageData = vtkSmartPointer<vtkImageData>::New();
		imageData->DeepCopy(shiftScale->GetOutput());

		m_ImageDataList.emplace_back(imageData);
	};

	if (m_ImageFileType == ImageFileType_NiTFI)
	{
		auto reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
		func(reader);

	}
	else if (m_ImageFileType == ImageFileType_VolumeDicom)
	{

		ReadVolumeDicom(fullPath);
		/*	auto reader = vtkSmartPointer<vtkDICOMImageReader>::New();
			func(reader);*/
	}
	else if (m_ImageFileType == ImageFileType_MetaImage)
	{
		auto reader = vtkSmartPointer<vtkMetaImageReader>::New();
		func(reader);
	}
	else if (m_ImageFileType == ImageFileType_SequenceFrame)
	{
		QString path = fullPath;
		if (decryption)
		{
			auto start = std::chrono::steady_clock::now();

			path = CodingHelper::DeCodingAES(fullPath);

			auto end = std::chrono::steady_clock::now();
			auto diff = end - start;

			qDebug() << "decode video use: " << std::chrono::duration <double>(diff).count() << " s" << endl;

		}

		ReadVideo(path);

		for (size_t i = 0; i < m_MaxFrameCount; i++)
		{
			GetFrameByIndex(i);
		}

		m_VideoCapture.release();

		if (decryption)
		{
			QFile file(path);
			file.remove();
		}

	}
	else if (m_ImageFileType == ImageFileType_SingleFrame)
	{
		ReadSingleFrame(fullPath);
	}

	m_CurFrameCount = 0;
	m_ImageDataList[0]->GetDimensions(m_Dims);
	m_ImageDataList[0]->GetSpacing(m_OriginalSpacing);

	return true;
}

bool LoadMgr::LoadMaskData(const QString& fullPath, bool decryption)
{
	g_GlobalVal.IsMaskChanged = true;

	auto suffix = QFileInfo(fullPath).completeSuffix();

	qDebug() << suffix;

	auto type = CheckImageType(suffix);

	if (type != ImageFileType_NiTFI && type != ImageFileType_MetaImage
		&& type != ImageFileType_MetaImage && type != ImageFileType_MaskData) return false;

	auto imageData = vtkSmartPointer<vtkImageData>::New();

	if (type != ImageFileType_MaskData)
	{
		auto reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
		reader->SetFileName(fullPath.toStdString().c_str());
		reader->Update();

		imageData->DeepCopy(reader->GetOutput());

		auto dim = imageData->GetDimensions();
		for (size_t i = 0; i < 2; i++)
		{
			if (dim[i] != m_Dims[i])
			{
				return false;
			}
		}

		if (m_ImageFileType != ImageFileType_VolumeDicom)
		{
			auto change = vtkSmartPointer<vtkImageChangeInformation>::New();
			change->SetInputData(imageData);
			change->SetOutputSpacing(1, 1, 1);
			change->Update();
			imageData = change->GetOutput();
		}

		if (decryption)
		{
			CodingHelper::DeCoding(imageData);
		}
		
		auto shiftScale = vtkSmartPointer<vtkImageShiftScale>::New();
		shiftScale->SetInputData(imageData);
		shiftScale->SetOutputScalarTypeToUnsignedChar();
		shiftScale->Update();

		imageData = shiftScale->GetOutput();
	}
	else
	{
		auto vox = m_ImageDataList[0];
		auto dim = vox->GetDimensions();
		auto space = vox->GetSpacing();
		auto extent = vox->GetExtent();
		auto origin = vox->GetOrigin();

		if (m_ImageFileType == ImageFileType_SequenceFrame)
		{
			dim[2] = m_MaxFrameCount;
			extent[5] = m_MaxFrameCount - 1;
		}

		imageData->SetDimensions(dim);
		imageData->SetSpacing(space);
		imageData->SetExtent(extent);
		imageData->SetOrigin(origin);
		imageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		int NumberOfPoints = imageData->GetNumberOfPoints();

		QFile f(fullPath);
		f.open(QFile::ReadOnly);
		QByteArray data = f.readAll();

		//auto file = gzopen(fullPath.toStdString().c_str(), "rb");
		//uint8 header[4];
		//gzread(file, header, 4);
		//int bufSize = header[0] | ((uint)header[1] << 8) 
		//	| ((uint)header[2] << 16) | ((uint)header[3] << 24);

		//uint8* buf = new uint8[bufSize];
		//gzread(file, buf, bufSize);
		//gzclose(file);

		//QByteArray bytes((char*)buf, bufSize);

		uint8 header[4];
		memcpy(header, data.data(), 4);
		size_t bufSize = header[0] | ((uint)header[1] << 8) 
			| ((uint)header[2] << 16) | ((uint)header[3] << 24);

		if (bufSize != NumberOfPoints) return false;

		data.remove(0, 4);

		auto start = std::chrono::steady_clock::now();

		auto deBytes = CodingHelper::DecodeBytes(data);

		auto end = std::chrono::steady_clock::now();
		auto diff = end - start;
		qDebug() << "decode use: " << std::chrono::duration <double>(diff).count() << " s" << endl;

		QByteArray bytes(bufSize, Qt::Initialization::Uninitialized);
		uLongf bytesSize = bufSize;
		uncompress((Bytef*)bytes.data(), &bytesSize, (Bytef*)deBytes.data(), deBytes.size());

		std::memcpy(imageData->GetScalarPointer(), bytes.data(), sizeof(unsigned char) * NumberOfPoints);

		qDebug() << "NumberOfPoints:" << NumberOfPoints;
		qDebug() << "read size:" << data.size();
		qDebug() << "deBytes size:" << deBytes.size();

		//delete [] buf;

		end = std::chrono::steady_clock::now();
		diff = end - start;
		qDebug() << "finish use: " << std::chrono::duration <double>(diff).count() << " s" << endl;
	}

	if (m_ImageFileType != ImageFileType_SequenceFrame)
	{
		m_MaskImageDataList[0]->DeepCopy(imageData);
	}
	else
	{
		auto dim = imageData->GetDimensions();
		size_t oneFrameSize = dim[0] * dim[1];
		for (size_t i = 0; i < m_MaskImageDataList.size(); i++)
		{
			auto dst = m_MaskImageDataList[i]->GetScalarPointer();
			auto src = (uint8*)imageData->GetScalarPointer();
			memcpy(dst, src + i * oneFrameSize, oneFrameSize * sizeof(uint8));
		}
	}

	return true;
}

vtkImageData* LoadMgr::GetImageData()
{
	if (m_ImageFileType == ImageFileType_MetaImage
		|| m_ImageFileType == ImageFileType_NiTFI
		|| m_ImageFileType == ImageFileType_VolumeDicom)
	{
		return m_ImageDataList[0];
	}

	return nullptr;
}

vtkImageData* LoadMgr::GetFrameByIndex(int index)
{
	if (index >= m_MaxFrameCount) return nullptr;

	assert(index <= m_ImageDataList.size());

	if (index == m_ImageDataList.size())
	{
		cv::Mat mat;
		m_VideoCapture >> mat;

		auto imageData = vtkSmartPointer<vtkImageData>::New();
		auto start = std::chrono::steady_clock::now();

		CVMatToVtkImageData(mat, false, imageData);

		auto end = std::chrono::steady_clock::now();
		auto diff = end - start;

		qDebug() << std::chrono::duration <double>(diff).count() << " s" << endl;

		m_ImageDataList.emplace_back(imageData);
	}

	m_CurFrameCount = index;

	return m_ImageDataList[index];
}

vtkImageData* LoadMgr::GetMaskDataByIndex(int index)
{
	assert(index < m_ImageDataList.size());

	auto it = m_MaskImageDataList.find(index);

	if (it == m_MaskImageDataList.end())
	{
		auto vox = m_ImageDataList[index];
		auto dim = vox->GetDimensions();
		auto space = vox->GetSpacing();
		auto extent = vox->GetExtent();
		auto origin = vox->GetOrigin();

		auto ImageData = vtkSmartPointer<vtkImageData>::New();
		ImageData->SetDimensions(dim);
		ImageData->SetSpacing(space);
		ImageData->SetExtent(extent);
		ImageData->SetOrigin(origin);
		ImageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		int NumberOfPoints = ImageData->GetNumberOfPoints();
		std::memset(ImageData->GetScalarPointer(), 0, sizeof(unsigned char) * NumberOfPoints);

		m_MaskImageDataList[index] = ImageData;
	}

	return m_MaskImageDataList[index];
}

vtkImageData* LoadMgr::GetCurMaskData()
{
	return GetMaskDataByIndex(m_CurFrameCount);
}

void LoadMgr::SaveMaskData(const QString& path)
{
	if (!g_GlobalVal.IsMaskChanged) return;

	auto ImageData = vtkSmartPointer<vtkImageData>::New();

	if (m_ImageDataList.empty()) return;
	if (m_ImageFileType == ImageFileType_SequenceFrame)
	{
		auto vox = m_ImageDataList[0];
		int dim[3];
		vox->GetDimensions(dim);
		auto space = vox->GetSpacing();
		int extent[6];
		vox->GetExtent(extent);
		auto origin = vox->GetOrigin();
		dim[2] = m_MaxFrameCount;
		extent[5] = dim[2] - 1;

		ImageData->SetDimensions(dim);
		ImageData->SetSpacing(space);
		ImageData->SetExtent(extent);
		ImageData->SetOrigin(origin);
		ImageData->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
		size_t NumberOfPoints = ImageData->GetNumberOfPoints();
		std::memset(ImageData->GetScalarPointer(), 0, sizeof(unsigned char) * NumberOfPoints);

		uint8* ptr = (uint8*)ImageData->GetScalarPointer();

		for (auto&& [k, v] : m_MaskImageDataList)
		{
			size_t oneFrameSize = dim[0] * dim[1];
			memcpy(ptr + k * oneFrameSize, v->GetScalarPointer(), sizeof(uint8) * oneFrameSize);
		}
	}
	else
	{
		//auto change = vtkSmartPointer<vtkImageChangeInformation>::New();
		//change->SetInputData(GetCurMaskData());
		//change->SetOutputSpacing(m_OriginalSpacing);
		//change->Update();



		ImageData->DeepCopy(GetCurMaskData());
	}

	//auto shiftScale = vtkSmartPointer<vtkImageShiftScale>::New();
	//shiftScale->SetInputData(ImageData);
	//shiftScale->SetOutputScalarTypeToUnsignedShort();
	//shiftScale->Update();
	//auto image_shift = shiftScale->GetOutput();

	auto path_str = path.toLocal8Bit().toStdString();
	std::stringstream ss;
	ss << path_str << "/" << g_GlobalVal.FileName.toLocal8Bit().toStdString() << "_Label" << ".mdat";
	
	//auto writer = vtkSmartPointer<vtkNIFTIImageWriter>::New();
	//writer->SetFileName(ss.str().c_str());
	//writer->SetInputData(ImageData);
	//writer->Update();

	size_t num = ImageData->GetNumberOfPoints();
	auto ptr = (char*)ImageData->GetScalarPointer();
	QByteArray bytes(ptr, num);
	QByteArray compBytes(num, Qt::Initialization::Uninitialized);
	uLongf compBytesSize = num;

	auto ok = compress2((Bytef*)compBytes.data(), &compBytesSize, (Bytef*)bytes.data(), bytes.size(), Z_BEST_SPEED);
	QByteArray enBytes(compBytes.data(), compBytesSize);

	qDebug() << "compress2 result:" << ok;
	qDebug() << "after compress2:" << compBytesSize;

	enBytes = CodingHelper::EncodeBytes(enBytes);
	
	qDebug() << "after encode:" << enBytes.size();

	uint bytesLen = num;
	uint8 buf[4];
	buf[0] = bytesLen & 0xFF;
	buf[1] = (bytesLen >> 8) & 0xFF;
	buf[2] = (bytesLen >> 16) & 0xFF;
	buf[3] = (bytesLen >> 24) & 0xFF;

	enBytes.insert(0, (char*)buf, 4);

	QString outPath = QString::fromStdString(ss.str());
	QFile outfile(outPath);
	outfile.open(QFile::WriteOnly | QFile::Truncate);
	outfile.write(enBytes);
	outfile.flush();
	outfile.close();

	//auto file = gzopen(ss.str().c_str(), "wb");

	//uint bytesLen = enBytes.size();
	//uint8 buf[4];
	//buf[0] = bytesLen & 0xFF;
	//buf[1] = (bytesLen >> 8) & 0xFF;
	//buf[2] = (bytesLen >> 16) & 0xFF;
	//buf[3] = (bytesLen >> 24) & 0xFF;

	//gzwrite(file, buf, 4);
	//gzwrite(file, enBytes.data(), enBytes.size());

	//gzclose(file);

	qDebug() << "scalar szie:" << num;
	qDebug() << "add header size:" << enBytes.size();
}

ImageFileType LoadMgr::CheckImageType(const QString& sff)
{
	ImageFileType type = ImageFileType_None;

	auto suffix = sff.toLower();

	if (suffix == "nii.gz" || suffix == "nii")
	{
		type = ImageFileType_NiTFI;
	}
	else if (suffix == "dcm")
	{
		type = ImageFileType_VolumeDicom;
	}
	else if (suffix == "mhd" || suffix == "mha")
	{
		type = ImageFileType_MetaImage;
	}
	else if (suffix == "bmp" || suffix == "jpg" || suffix == "png")
	{
		type = ImageFileType_SingleFrame;
	}
	else if (suffix == "mp4" || suffix == "avi")
	{
		type = ImageFileType_SequenceFrame;
	}
	else if (suffix == "mdat")
	{
		type = ImageFileType_MaskData;
	}
	else
	{
		type = ImageFileType_VolumeDicom;
	}

	return type;
}

void LoadMgr::ReadVideo(const QString& fullPath)
{
	m_VideoCapture.open(fullPath.toStdString());

	m_MaxFrameCount = m_VideoCapture.get(CAP_PROP_FRAME_COUNT);
}

void LoadMgr::ReadSingleFrame(const QString& fullPath)
{
	cv::Mat mat = cv::imread(fullPath.toLocal8Bit().toStdString());
	auto imageData = vtkSmartPointer<vtkImageData>::New();
	CVMatToVtkImageData(mat, false, imageData);
	m_ImageDataList.emplace_back(imageData);

	m_MaxFrameCount = 1;
}

void LoadMgr::ReadVolumeDicom(const QString& fullPath)
{
	using ImageType = itk::Image<uint8, 3>;
	auto reader = itk::ImageFileReader< ImageType >::New();
	using ImageIOType = itk::GDCMImageIO;
	ImageIOType::Pointer dicomIO = ImageIOType::New();

	reader->SetFileName(fullPath.toLocal8Bit().toStdString());
	reader->SetImageIO(dicomIO);
	reader->Update();

	
	std::string seriseNumberTag = "0020|0011";

	auto getTagFunc = [&](const std::string& seriseNumberTag, const std::string& desc) {
		auto& meta = reader->GetMetaDataDictionary();
		auto it = meta.Find(seriseNumberTag);
		if (it != meta.End())
		{
			auto tag = (itk::MetaDataObject<std::string>*)meta.Get(seriseNumberTag);
			auto seriseNumber = tag->GetMetaDataObjectValue();

			return std::pair(QString::fromStdString(desc), QString::fromStdString(seriseNumber));
		}
		return std::pair(QString::fromStdString(desc), QString::fromStdString(""));
	};
	m_DicomTags.emplace(getTagFunc("0010|0010", "PatientName"));
	m_DicomTags.emplace(getTagFunc("0008|2127", "ViewName"));
	m_DicomTags.emplace(getTagFunc("0028|0030", "PixelSpacing"));


	auto filter = itk::ImageToVTKImageFilter< ImageType >::New();
	filter->SetInput(reader->GetOutput());
	filter->Update();

	/*auto change = vtkSmartPointer<vtkImageChangeInformation>::New();
	change->SetInputData(filter->GetOutput());
	change->SetOutputSpacing(1, 1, 1);
	change->Update();*/
	//auto reslice = vtkSmartPointer<vtkImageReslice>::New();
	//reslice->SetOutputDimensionality(3);
	//reslice->SetOutputSpacing(1, 1, 1);
	//reslice->SetInputData(filter->GetOutput());
	//reslice->Update();

	//auto shiftScale = vtkSmartPointer<vtkImageShiftScale>::New();
	//shiftScale->SetInputData(reslice->GetOutput());
	//shiftScale->SetOutputScalarTypeToUnsignedChar();
	//shiftScale->Update();

	auto imageData = vtkSmartPointer<vtkImageData>::New();
	imageData->DeepCopy(filter->GetOutput());
	auto dim = imageData->GetDimensions();
	auto range = imageData->GetScalarRange();
	auto num = imageData->GetNumberOfPoints();
	auto p = (unsigned char*)imageData->GetScalarPointer();

	std::map<int, int> hist;
	for (size_t i = 0; i < dim[0] * dim[1]; i++)
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

	auto cmp = [](auto& l, auto& r)
	{
		return l.second > r.second;
	};

	std::set<std::pair<int, int>, decltype(cmp)> m(hist.begin(), hist.end(), cmp);

	uint8 bgVal = m.begin()->first;
	int max = range[1];
	float d = max - bgVal;

	for (size_t i = 0; i < num; i++)
	{
		p[i] = p[i] < bgVal ? 0 : (p[i] - bgVal) / d * 255;
	}


	if (g_ConfigVal.IsLoadDicomToVideo)
	{
		auto ptr = (uint8*)imageData->GetScalarPointer();
		
		m_ImageFileType = ImageFileType_SequenceFrame;
		m_MaxFrameCount = dim[2];
		size_t sliceLen = dim[0] * dim[1];

		for (size_t i = 0; i < m_MaxFrameCount; i++)
		{
			auto imageSlice = vtkSmartPointer<vtkImageData>::New();

			imageSlice->SetSpacing(1, 1, 1);
			imageSlice->SetOrigin(0, 0, 0);
			imageSlice->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, 0);
			imageSlice->SetDimensions(dim[0], dim[1], 1);
			imageSlice->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

			memcpy(imageSlice->GetScalarPointer(), ptr + sliceLen * i, sliceLen);
			m_ImageDataList.emplace_back(imageSlice);

		}
		
	}
	else
	{
		m_ImageDataList.emplace_back(imageData);
	}
}

void LoadMgr::CVMatToVtkImageData(const cv::Mat& sourceCVImage, bool flipOverXAxis, vtkImageData* outputVtkImage)
{
	//vtkSmartPointer<vtkImageData> outputVtkImage = vtkSmartPointer<vtkImageData>::New();
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

	outputVtkImage->Modified();
	//imageData->DeepCopy(outputVtkImage)
}
