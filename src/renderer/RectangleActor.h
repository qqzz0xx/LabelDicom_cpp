#pragma once
#include "SceneNode.h"

namespace NRender
{
	class RectangleActor : public SceneNode
	{
	public:

		RectangleActor();
		~RectangleActor();

		virtual void SetPoints(const glm::dvec3& p1, const glm::dvec3& p2);

		vtkAssembly* m_Assembly;

		glm::dvec3 m_Points[2];
	};

}