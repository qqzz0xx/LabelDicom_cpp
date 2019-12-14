#pragma once

#include "ModelBase.h"


struct  MaskEditRecordItem
{
	bool IsInvert;
	int LabelType;
	uint8* MaskPtr;
	std::vector<uint8> OldValues;
	std::vector<uint32> Indexs;
};

class MaskEditRecordModel : public ModelBase
{
public:
	MaskEditRecordModel();

	void AddRecord(MaskEditRecordItem&& record);
	void Undo();
	void Redo();


	virtual void Clear() override;

private:
	std::vector<MaskEditRecordItem> m_Records;

	std::vector<MaskEditRecordItem> m_UndoRecords;
};

