#pragma once

#include "Interface.h"
#include "BaseSliceTools.h"

class LocationTools : public BaseSliceTools
{
public:
	// Í¨¹ý IMprEventListener ¼Ì³Ð
	//virtual void OnMouseEvent(MprComponent* mpr, SliceComponent* slice, MouseEventType type) override;
	//virtual void OnSliderValueChange(MprComponent* mpr, SliceComponent* slice, int value) override;
	//virtual void OnMouseEvent(FrameComponent* slice, MouseEventType type) override;

	virtual void OnMouseEvent(MouseEventType type) override;

	std::function<void(SliceView* view, double* imgXY)> LocationCallback = nullptr;
	virtual void SetEnable(bool v) override;

protected:
	void UpdateLocation();
	void UpdateMprByVoxPos(MprComponent* mpr, SliceComponent* slice, double* vPos);
};

