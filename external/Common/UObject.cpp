#include "UObject.h"



UObject::UObject()
{
	m_UUID = GenUUID();
}


UObject::~UObject()
{
}

const std::string UObject::GetUUID() const
{
	return m_UUID;
}

void UObject::SetUUID(const std::string & uuid)
{
	m_UUID = uuid;
}

const std::string UObject::GetName() const
{
	return m_Name;
}

void UObject::SetName(const std::string & name)
{
	m_Name = name;
}

bool UObject::IsEqual(const UObject & a)
{
	return m_UUID == a.GetUUID();
}

bool UObject::IsEqualName(const UObject & b)
{
	return m_Name == b.GetName();
}



