#pragma once
#include "BaseSliceTools.h"

class WinLevelWidthTools : public BaseSliceTools
{
public:
	WinLevelWidthTools();
	~WinLevelWidthTools();

	virtual void OnMouseEvent(MouseEventType type) override;

	std::function<void(SliceView*, int, int)> OnWinLevelWidthCallback = nullptr;

};

