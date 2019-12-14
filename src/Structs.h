#pragma once
#include "Header.h"
#include <QStandardItem>
#include <QListWidgetItem>

class PolygonPoint
{
public:
	glm::dvec3 Pos = {};
	bool Selcet = false;
};

struct QStandardItemUUID : public QStandardItem
{
	std::string uuid;
};

struct QListWidgetItemWithPath : public QListWidgetItem
{
	QString Path;
};