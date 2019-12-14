#include "PolygonActor.h"
#include "PointActor.h"
#include "LineActor.h"
#include "Structs.h"
#include "GlobalVal.h"

NRender::PolygonActor::PolygonActor()
{
}

NRender::PolygonActor::~PolygonActor()
{
}

void NRender::PolygonActor::AddPoint(const glm::dvec3& pos)
{
	auto p = std::make_shared<PointActor>();
	p->SetRadius(m_InitPointRadius * g_GlobalVal.PolyActorZoomFactor);
	p->SetPosition((double*)glm::value_ptr(pos));
	double rgb[3] = { 1, 0.9, 0.6 };
	p->SetColor(rgb);
	AddChild(p);

	if (!m_Points.empty())
	{
		auto line = std::make_shared<LineActor>();
		auto p2 = m_Points.back();
		line->SetPos2((double*)glm::value_ptr(pos));
		line->SetPos1(p2->GetActor()->GetPosition());
		line->SetLineWidth(m_InitLineWidth);
		double rgb[3] = { 0.3, 1.0, 0.5 };
		line->SetColor(rgb);
		
		AddChild(line);
		m_Lines.emplace_back(line);
	}

	m_Points.emplace_back(p);
}

void NRender::PolygonActor::RemovePoint()
{
	RemoveChild(m_Points.back());
	m_Points.pop_back();
}

void NRender::PolygonActor::Clear()
{
	for (auto& var : m_ChildNodes)
	{
		var->Exit();
	}

	m_ChildNodes.clear();
	m_Points.clear();
	m_Lines.clear();
}

void NRender::PolygonActor::UpdatePoint(int idx, const glm::dvec3& pos)
{
	auto p = (double*)glm::value_ptr(pos);
	m_Points[idx]->SetPosition(p);

	int preIdx = idx - 1;
	preIdx = preIdx < 0 ? m_Lines.size() - 1 : preIdx;

	m_Lines[preIdx]->SetPos2(p);
	m_Lines[idx]->SetPos1(p);
}

void NRender::PolygonActor::UpdateAll(const std::vector<glm::dvec3>& list)
{
	for (auto& var : m_ChildNodes)
	{
		var->Exit();
	}
	m_ChildNodes.clear();

	for (auto& var : list)
	{
		AddPoint(var);
	}

	AddLastLine();
}

void NRender::PolygonActor::UpdateAll(const std::vector<PolygonPoint>& list)
{
	for (auto& var : m_ChildNodes)
	{
		var->Exit();
	}
	m_ChildNodes.clear();
	m_Lines.clear();
	m_Points.clear();

	for (auto& var : list)
	{
		AddPoint(var.Pos);
	}

	AddLastLine();
}

void NRender::PolygonActor::AddLastLine()
{
	if (m_Points.size() >= 2)
	{
		auto line = std::make_shared<LineActor>();
		auto p1 = m_Points.back();
		auto p2 = m_Points.front();
		line->SetPos1(p1->GetActor()->GetPosition());
		line->SetPos2(p2->GetActor()->GetPosition());
		line->SetLineWidth(m_InitLineWidth);
		double rgb[3] = { 0.3, 1.0, 0.5 };
		line->SetColor(rgb);
		AddChild(line);
		m_Lines.emplace_back(line);
	}
}

void NRender::PolygonActor::OnCameraZoom(double factor)
{
	for (auto&& var : m_Points)
	{
		var->SetRadius(m_InitPointRadius*g_GlobalVal.PolyActorZoomFactor);
	}
}
