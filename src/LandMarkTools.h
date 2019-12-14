#pragma once
#include "BaseSliceTools.h"

namespace NRender
{
	class LandMarkActor;
	class LineActor;
}

class LandMarkTools : public BaseSliceTools
{
public:
	LandMarkTools();

	//virtual void OnMouseEvent(MprComponent* mpr, SliceComponent* slice, MouseEventType type) override;

	//virtual void OnSliderValueChange(MprComponent* mpr, SliceComponent* slice, int value) override;

	//virtual void OnMouseEvent(FrameComponent* slice, MouseEventType type) override;


	virtual void OnMouseEvent(MouseEventType type) override;
	virtual void SetEnable(bool v) override;


	virtual void SetCurIndex(int index) override;

private:

	NRender::LandMarkActor* m_PickedActor = nullptr;
	//std::weak_ptr<NRender::LandMarkActor> m_CurLandMarkActor;
	std::weak_ptr<NRender::LineActor> m_CurLineActor;

};

