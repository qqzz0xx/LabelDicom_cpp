#pragma once
#include "Header.h"
#include "Interface.h"
#include "BaseSliceTools.h"
#include "BoundingBoxActor.h"

using namespace NRender;

class BoundingBoxTools : public BaseSliceTools
{
public:
	virtual void OnMouseEvent(MouseEventType type) override;

	virtual void SetEnable(bool v) override;

protected:
	enum State
	{
		None,
		Start,
		Drag,
		Editor,
	};

	State m_State = State::None;
	glm::dvec3 m_BoxPoints[2];
	std::shared_ptr<RectangleActor> m_RectangleActor;
};

