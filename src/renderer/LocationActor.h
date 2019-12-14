#pragma once

#include "Behavior.h"
#include "SceneNode.h"
#include "Types.h"
#include "LineActor.h"

namespace NRender
{
	class LocationActor : public SceneNode
	{
	public:
		LocationActor();

		void SetXAxisColor(double* rgb);
		void SetYAxisColor(double* rgb);

		void SetColorOfType(MprType type);

		LineActor m_AxisX;
		LineActor m_AxisY;

		virtual void OnMouseEvent(MouseEventType type) override;

	};
}

