#include "BoundingBox3DActor.h"
#include "BoundingBox3DLabelModel.h"
#include "vtkTextActor3D.h"

void NRender::BoundingBox3DActor::Init(std::shared_ptr<BoundingBox3DLabelItem> item)
{
	m_Model = item;
	double rgba[4];
	QtHelper::QColorToVTKRGBA(item->LabelType->Color, rgba);
	m_PlaneActor->GetProperty()->SetColor(rgba);
	m_PlaneActor->GetProperty()->SetOpacity(rgba[3] * 0.6);
	m_TextActor->SetVisibility(false);
}

void NRender::BoundingBox3DActor::InitPoints(const glm::dvec3& p1, const glm::dvec3& p2)
{
	__super::SetPoints(p1, p2);
}

void NRender::BoundingBox3DActor::UpdateColor()
{
	double rgba[4];
	QtHelper::QColorToVTKRGBA(m_Model->LabelType->Color, rgba);
	m_PlaneActor->GetProperty()->SetColor(rgba);
	m_PlaneActor->GetProperty()->SetOpacity(rgba[3] * 0.6);
}

void NRender::BoundingBox3DActor::SetPoints(const glm::dvec3& p1, const glm::dvec3& p2)
{
	__super::SetPoints(p1, p2);

	if (ChangedCallback)
	{
		ChangedCallback(this);
	}
}
