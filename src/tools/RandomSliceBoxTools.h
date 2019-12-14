#pragma once
#include "BoundingBoxTools.h"
namespace NRender
{
	class BoxWidgetActor;
	class SliceCutterActor;
}

struct RandomBoxLabelItem;

class RandomSliceBoxTools : public BoundingBoxTools
{
public:
	virtual void OnMouseEvent(MouseEventType type) override;
	virtual void SetEnable(bool v) override;

	void OnAccpet();
	void OnRotatePlanetToX();
	void OnRotatePlanetToY();
	void OnRotatePlanetToZ();
	void OnClear();

	//std::shared_ptr<NRender::BoxWidgetActor> m_BoxActor;
	//std::shared_ptr<NRender::SliceCutterActor> m_Cutter;
	static std::shared_ptr<RandomBoxLabelItem> m_LabelItem;
};

