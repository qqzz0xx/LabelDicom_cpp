#pragma once
#include "Header.h"
#include "Interface.h"

class MprComponent;
class FrameComponent;
class MprComponent;

class BaseMprTools : public IMprEventListener, public UObject
{
public:
	// Í¨¹ý IMprEventListener ¼Ì³Ð
	virtual void OnMouseEvent(MprComponent* mpr, SliceComponent* slice, MouseEventType type) override;
	virtual void OnMouseEvent(FrameComponent* slice, MouseEventType type) override;
	virtual void OnSliderValueChange(MprComponent* mpr, SliceComponent* slice, int value) override;
	virtual void SetEnable(bool v) { m_Enable = v; }
	virtual bool GetEnable() { return m_Enable; }
protected:
	bool m_Enable = true;
};

