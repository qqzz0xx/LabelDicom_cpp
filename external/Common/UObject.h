#pragma once
#include <string>
#include "uuid/uuid4.h"

class UObject
{
public:
	UObject();
	~UObject();
	const std::string GetUUID() const;
	void SetUUID(const std::string& uuid);
	const std::string GetName() const;
	void SetName(const std::string& name);

	bool IsEqual(const UObject& b);
	bool IsEqualName(const UObject& b);

protected:
	std::string m_UUID;
	std::string m_Name;
};

