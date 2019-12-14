#pragma once

#include "Types.h"

class SliceComponent;
class FrameComponent;
class MprComponent;

class IEnterExit
{
public:
	virtual void Enter() = 0;
	virtual void Exit() = 0;

};

class IRunable
{
public:
	virtual void Run() = 0;
};

class IEventListener
{
public:
	virtual void OnMouseEvent(MouseEventType type) = 0;
};

class IMprEventListener
{
public:
	virtual void OnMouseEvent(MprComponent* mpr, SliceComponent* slice, MouseEventType type) = 0;
	virtual void OnMouseEvent(FrameComponent* slice, MouseEventType type) = 0;
	virtual void OnSliderValueChange(MprComponent* mpr, SliceComponent* slice, int value) = 0;
};
