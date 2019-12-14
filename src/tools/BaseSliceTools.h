#pragma once
#include "Header.h"
#include "Interface.h"
class SliceView;

class BaseSliceTools : public IEventListener
{
public:
	BaseSliceTools();
	~BaseSliceTools();

	virtual void Init(SliceView* view) { m_View = view; }
	// Í¨¹ý IEventListener ¼Ì³Ð
	virtual void OnMouseEvent(MouseEventType type) override;
	virtual void SetEnable(bool v) { m_Enable = v; }
	virtual bool GetEnable() { return m_Enable; }
	virtual void SetCurIndex(int index);

protected:
	bool IsLeftDrag(MouseEventType type);
	bool IsRightDrag(MouseEventType type);
	bool IsMiddleDrag(MouseEventType type);
	int m_CurIndex = 0;
	bool m_Enable = true;
	SliceView* m_View = nullptr;
};

