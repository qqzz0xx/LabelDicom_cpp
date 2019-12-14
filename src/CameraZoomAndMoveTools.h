#pragma once
#include "Header.h"
#include "BaseSliceTools.h"

class CameraZoomAndMoveTools : public BaseSliceTools
{
public:
	virtual void OnMouseEvent(MouseEventType type) override;

	std::function<void(double)> ZoomChangedCallback = nullptr;
private:
	double m_Factor = 0.05;
};

