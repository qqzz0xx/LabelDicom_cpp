#pragma once
#include "BoundingBoxActor.h"

struct BoundingBox3DLabelItem;

namespace NRender
{
	class BoundingBox3DActor : public BoundingBoxActor
	{
	public:
		void Init(std::shared_ptr<BoundingBox3DLabelItem> item);
		std::function<void(BoundingBox3DActor * actor)> ChangedCallback;
		void InitPoints(const glm::dvec3& p1, const glm::dvec3& p2);
		void UpdateColor();

	protected:
		virtual void SetPoints(const glm::dvec3& p1, const glm::dvec3& p2) override;

	private:
		std::shared_ptr<BoundingBox3DLabelItem> m_Model;
	};
}