#include "LineActor.h"
#include "vtkTextProperty.h"
#include "RenderEngine.h"

using namespace NRender;

LineActor::LineActor()
{
	m_Name = "LineActor";
	auto line = InitActor(vtkActor::New());
	m_LineSrc = vtkSmartPointer<vtkLineSource>::New();
	auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(m_LineSrc->GetOutput());
	mapper->Update();
	line->SetMapper(mapper);
	//line->GetProperty()->SetLineWidth(0.5);

}


LineActor::~LineActor()
{
}

void NRender::LineActor::SetPos1(double * p1)
{
	m_LineSrc->SetPoint1(p1);
	m_LineSrc->Update();
}

void NRender::LineActor::SetPos2(double * p2)
{
	m_LineSrc->SetPoint2(p2);
	m_LineSrc->Update();
	UpdateDist();
}

void NRender::LineActor::SetLineWidth(double w)
{
	auto p = vtkActor::SafeDownCast(m_Actor);
	p->GetProperty()->SetLineWidth(w);
	UpdateDist();
}

void NRender::LineActor::UpdateDist()
{
	if (m_TextActor)
	{
		auto p1 = glm::make_vec3(m_LineSrc->GetPoint1());
		auto p2 = glm::make_vec3(m_LineSrc->GetPoint2());

		auto c = (p1 + p2) * 0.5;
		m_TextActor->SetPosition(glm::value_ptr(c));

		auto len = glm::length(p1 - p2);
		std::stringstream ss;
		ss << len << " px";
		m_TextActor->SetInput(ss.str().c_str());
	}
}

void NRender::LineActor::SetActive(bool isShow)
{
	__super::SetActive(isShow);
	if (m_TextActor)
	{
		m_TextActor->SetVisibility(isShow);
	}
}

void NRender::LineActor::SetEnableDist(bool v)
{
	if (v)
	{
		if (m_TextActor == nullptr)
		{
			m_TextActor = vtkSmartPointer<vtkTextActor3D>::New();
			m_TextActor->SetInput("LandMark");
			m_TextActor->SetPosition(0, 10, 0);
			auto prop = m_TextActor->GetTextProperty();
			prop->SetFontFamilyToCourier();
			prop->SetBold(1);
			prop->SetFontSize(18);
			prop->SetColor(1, 0, 0);
			prop->SetOpacity(0.99);

			if (m_RenderEngine)
			{
				m_RenderEngine->GetRenderer()->AddViewProp(m_TextActor);
			}
		}
	}
	else
	{
		if (m_TextActor)
		{
			if (m_RenderEngine)
			{
				m_RenderEngine->GetRenderer()->RemoveActor(m_Actor);
			}
		}
	}
}

double NRender::LineActor::GetLength()
{
	return m_LineSrc->GetPoint2()[0];
}

void NRender::LineActor::SetLength(double len)
{
	m_Length = len;
	Refresh();
}

void NRender::LineActor::Refresh()
{
	double p1[3] = { 0 };
	double p2[3] = { m_Length, 0, 0 };

	m_LineSrc->SetPoint1(p1);
	m_LineSrc->SetPoint2(p2);
	m_LineSrc->Update();
}


