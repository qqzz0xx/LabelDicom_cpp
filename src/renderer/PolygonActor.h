#pragma once
#include "SceneNode.h"

class PolygonPoint;

namespace NRender
{
	class PointActor;
	class LineActor;
	class PolygonActor : public SceneNode
	{
	public:
		PolygonActor();
		~PolygonActor();

		void AddPoint(const glm::dvec3& pos);
		void RemovePoint();
		void Clear();
		void UpdatePoint(int idx, const glm::dvec3& pos);
		void UpdateAll(const std::vector<glm::dvec3>& list);
		void UpdateAll(const std::vector<PolygonPoint>& list);
		std::shared_ptr<PointActor>& GetPoint(int idx) { return m_Points[idx]; }
		void AddLastLine();
		virtual void OnCameraZoom(double factor) override;

	private:
		std::vector<std::shared_ptr<PointActor>> m_Points;
		std::vector<std::shared_ptr<LineActor>> m_Lines;
		

		double m_InitPointRadius = 2.0;
		double m_InitLineWidth = 2.0;
	};

}