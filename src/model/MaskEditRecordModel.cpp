#include "MaskEditRecordModel.h"

MaskEditRecordModel::MaskEditRecordModel()
{
	m_Name = typeid(this).name();
}

void MaskEditRecordModel::AddRecord(MaskEditRecordItem&& record)
{
	m_Records.emplace_back(record);
	m_UndoRecords.clear();

	if (m_Records.size() > 10)
	{
		m_Records.erase(m_Records.begin());
	}
}

void MaskEditRecordModel::Undo()
{
	if (m_Records.empty()) return;
	auto&& record = m_Records.back();

	for (size_t i = 0, size = record.Indexs.size(); i < size; i++)
	{
		auto idx = record.Indexs[i];
		auto val = record.OldValues[i];

		record.MaskPtr[idx] = val;
	}

	m_UndoRecords.emplace_back(record);
	m_Records.pop_back();
}

void MaskEditRecordModel::Redo()
{
	if (m_UndoRecords.empty()) return;
	auto&& record = m_UndoRecords.back();

	for (size_t i = 0, size = record.Indexs.size(); i < size; i++)
	{
		auto idx = record.Indexs[i];

		record.MaskPtr[idx] = record.LabelType;
	}

	m_Records.emplace_back(record);
	m_UndoRecords.pop_back();
}

void MaskEditRecordModel::Clear()
{
	m_Records.clear();
	m_UndoRecords.clear();
}
