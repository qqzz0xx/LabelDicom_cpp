#include "BoundingBoxActor.h"
#include "vtkRegularPolygonSource.h"
#include "RenderEngine.h"
#include "ModelManager.h"
#include "AppMain.h"
#include "MprPositionModel.h"
#include "vtkBoundingBox.h"
#include "BoundingBoxLabelModel.h"
#include "vtkPlaneSource.h"
#include "QtHelper.h"
#include "vtkTextActor3D.h"
#include "vtkTextProperty.h"
#include "Helper.h"

#include "glm.hpp"
#include "ext.hpp"


NRender::BoundingBoxActor::BoundingBoxActor() : RectangleActor()
{
	m_Name = "BoundingBoxActor";

	auto func = [&](int i)
	{
		auto m_Source = vtkSmartPointer<vtkRegularPolygonSource>::New();

		//polygonSource->GeneratePolygonOff(); // Uncomment this line to generate only the outline of the circle
		m_Source->SetNumberOfSides(10);
		m_Source->SetRadius(3);
		m_Source->SetCenter(0, 0, 0);

		auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(m_Source->GetOutputPort());

		auto m_Circle = vtkSmartPointer<vtkActor>::New();
		m_Circle->SetMapper(mapper);
		//m_Circle->SetPosition(p);
		m_Circle->GetProperty()->SetColor(1, 1, 0);
		m_Circle->GetProperty()->SetOpacity(0.8);
		m_BoundingActors[i] = m_Circle;
	};

	for (size_t i = 0; i < 8; i++)
	{
		func(i);
	}

	m_PlaneSrc = vtkSmartPointer<vtkPlaneSource>::New();
	m_PlaneSrc->Update();

	m_PlaneActor = vtkSmartPointer<vtkActor>::New();
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_PlaneSrc->GetOutput());
	mapper->Update();

	m_PlaneActor->SetMapper(mapper);
	m_PlaneActor->GetProperty()->SetOpacity(0.6);

	m_TextActor = vtkSmartPointer<vtkTextActor3D>::New();
	m_TextActor->SetInput("LandMark");
	m_TextActor->SetPosition(0, 10, 0);
	auto prop = m_TextActor->GetTextProperty();
	//prop->SetFontFamilyToCourier();
	Helper::SetVtkFont(prop);
	//prop->SetBold(1);
	prop->SetFontSize(18);
	prop->SetColor(0, 0, 0);
	prop->SetOpacity(0.99);



}

void NRender::BoundingBoxActor::Init(std::shared_ptr<BoundingBoxLabelItem> item)
{
	m_Model = item;
	m_TextActor->SetInput(item->LabelType->Desc.toStdString().c_str());
	UpdateColor();
}

void NRender::BoundingBoxActor::Start()
{
	__super::Start();
	m_RenderEngine->GetRenderer()->AddActor(m_PlaneActor);
	m_RenderEngine->GetRenderer()->AddActor(m_TextActor);

	for (auto&& var : m_BoundingActors)
	{
		if (var && m_RenderEngine)
		{
			m_RenderEngine->GetRenderer()->AddActor(var);
		}
	}
}

void NRender::BoundingBoxActor::Exit()
{
	__super::Exit();

	for (auto&& var : m_BoundingActors)
	{
		if (var && m_RenderEngine)
		{
			m_RenderEngine->GetRenderer()->RemoveActor(var);
		}
	}
	m_RenderEngine->GetRenderer()->RemoveActor(m_PlaneActor);
	m_RenderEngine->GetRenderer()->RemoveActor(m_TextActor);
}

void NRender::BoundingBoxActor::Update(float dt)
{

}

void NRender::BoundingBoxActor::UpdateColor()
{
	double rgba[4];
	QtHelper::QColorToVTKRGBA(m_Model->LabelType->Color, rgba);
	m_PlaneActor->GetProperty()->SetColor(rgba);
	m_PlaneActor->GetProperty()->SetOpacity(rgba[3] * 0.6);

	m_TextActor->GetTextProperty()->SetColor(rgba);
	m_TextActor->GetTextProperty()->SetOpacity(glm::pow(rgba[3] * 0.6, 0.4));
}

void NRender::BoundingBoxActor::SetPoints(const glm::dvec3& p1, const glm::dvec3& p2)
{
	__super::SetPoints(p1, p2);

	SetBounds(p1, p2);

	if (m_Model)
	{
		auto center = (p1 + p2) * 0.5;
		auto d = glm::abs(p2 - p1) * 0.5;
		auto pos = center + glm::dvec3(-d.x, d.y, 0);
		m_TextActor->SetPosition(glm::value_ptr(pos));
	}
}

void NRender::BoundingBoxActor::OnMouseEvent(MouseEventType type)
{
	auto model = gModelMgr->GetModel<MprPositionModel>();
	auto p = glm::make_vec3(model->m_WorldPos);
	p.z = 1;

	bool isChanged = false;

	if (type == MouseEventType_LeftDown)
	{
		if (m_State == State::None)
		{
			m_DragPointIdx = CheckPick(p);
			if (m_DragPointIdx != -1)
			{
				m_State = State::DragPoint;
				m_StartPos = p;
				if (m_DragPointIdx < 4)
				{
					m_DragBoxPos[0] = m_Bounds[m_DragPointIdx];
					m_DragBoxPos[1] = m_Bounds[(m_DragPointIdx + 2) % 4];
				}
				if (m_DragPointIdx == 4 || m_DragPointIdx == 7)
				{
					m_DragBoxPos[0] = m_Bounds[0];
					m_DragBoxPos[1] = m_Bounds[2];
				}
				if (m_DragPointIdx == 5 || m_DragPointIdx == 6)
				{
					m_DragBoxPos[0] = m_Bounds[2];
					m_DragBoxPos[1] = m_Bounds[0];
				}
			}
			else
			{
				if (CheckHitBox(p))
				{
					m_State = State::Move;
					m_StartPos = p;
				}
			}
		}
	}
	else if (type == MouseEventType_Move)
	{
		auto d = p - m_StartPos;

		if (m_State == State::Move)
		{
			SetPoints(m_Points[0] + d, m_Points[1] + d);
			isChanged = true;
		}
		else if (m_State == State::DragPoint)
		{
			auto d = p - m_StartPos;

			if (m_DragPointIdx < 4)
			{

			}
			else if (m_DragPointIdx == 4 || m_DragPointIdx == 6)
			{
				d = glm::dvec3(d.x, 0, 0);
			}
			else if (m_DragPointIdx == 5 || m_DragPointIdx == 7)
			{
				d = glm::dvec3(0, d.y, 0);
			}

			m_DragBoxPos[0] += d;
			SetPoints(m_DragBoxPos[0], m_DragBoxPos[1]);
			isChanged = true;
		}

		m_StartPos = p;
	}
	else if (type == MouseEventType_LeftUp)
	{
		m_State = State::None;
	}

	if (isChanged)
	{
		m_RenderEngine->Render();
	}
}

bool NRender::BoundingBoxActor::CheckHitBox(const glm::dvec3& pos)
{
	glm::dvec3 offset = glm::normalize(m_Points[0] - m_Points[1]) * 5;
	vtkBoundingBox box;
	box.AddPoint((double*)glm::value_ptr(m_Points[0] + offset));
	box.AddPoint((double*)glm::value_ptr(m_Points[1] - offset));

	bool hit = box.ContainsPoint((double*)glm::value_ptr(pos));
	return hit;
}

void NRender::BoundingBoxActor::SetEnable(bool isEnable)
{
	__super::SetEnable(isEnable);

	double color[3] = { 1,0,0 };
	if (isEnable)
	{
		color[1] = 1;
	}

	for (auto&& var : m_BoundingActors)
	{
		if (var)
		{
			var->GetProperty()->SetColor(color);
		}
	}
}

void NRender::BoundingBoxActor::SetActive(bool isShow)
{
	__super::SetActive(isShow);
	
	if (m_PlaneActor)
	{
		m_PlaneActor->SetVisibility(isShow);
	}
	if (m_TextActor)
	{
		m_TextActor->SetVisibility(isShow);
	}

	for (auto&& var : m_BoundingActors)
	{
		if (var)
		{
			var->SetVisibility(isShow);
		}
	}
}

int NRender::BoundingBoxActor::CheckPick(const glm::dvec3& p)
{
	for (size_t i = 0; i < 8; i++)
	{
		if (glm::length(p - m_Bounds[i]) < 4)
		{
			return i;
		}
	}

	return -1;
}

void NRender::BoundingBoxActor::SetBounds(const glm::dvec3& p1, const glm::dvec3& p2)
{
	m_Bounds[0] = p1;
	m_Bounds[2] = p2;

	m_Bounds[1] = glm::dvec3(p1.x, p2.y, p1.z);
	m_Bounds[3] = glm::dvec3(p2.x, p1.y, p2.z);

	m_Bounds[4] = (m_Bounds[0] + m_Bounds[1]) * 0.5;
	m_Bounds[5] = (m_Bounds[1] + m_Bounds[2]) * 0.5;
	m_Bounds[6] = (m_Bounds[2] + m_Bounds[3]) * 0.5;
	m_Bounds[7] = (m_Bounds[3] + m_Bounds[0]) * 0.5;

	for (size_t i = 0; i < 8; i++)
	{
		m_BoundingActors[i]->SetPosition(glm::value_ptr(m_Bounds[i]));
	}

	if (m_Model)
	{
		m_Model->Bounding[0] = p1;
		m_Model->Bounding[1] = p2;
	}

	m_PlaneSrc->SetPoint1((double*)glm::value_ptr(p1));
	m_PlaneSrc->SetPoint2((double*)glm::value_ptr(p2));
	m_PlaneSrc->SetOrigin((double*)glm::value_ptr(m_Bounds[1]));
	m_PlaneSrc->Update();
}

