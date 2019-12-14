#pragma once
#include "BoundingBoxTools.h"

class BoundingBox3DTools : public BoundingBoxTools
{
public:
	virtual void OnMouseEvent(MouseEventType type) override;
	

	virtual void SetEnable(bool v) override;

};

