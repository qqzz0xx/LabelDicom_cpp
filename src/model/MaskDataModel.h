#pragma once

#include "Header.h"
#include "ModelBase.h"
#include <QStandardItemModel>

struct MaskDataItem : public UObject
{
	int ImageIndex;
	vtkSmartPointer<vtkImageData> Data;
};

class MaskDataModel : public ModelBase
{
public:
	void Add(std::shared_ptr<MaskDataItem> p);

private:
	std::vector<std::shared_ptr<MaskDataItem>> m_Masks;
};

