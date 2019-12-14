#include "TextActor2D.h"
#include "RenderEngine.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"
#include "QtHelper.h"
#include "Helper.h"

NRender::TextActor2D::TextActor2D()
{
	m_Actor = vtkSmartPointer<vtkTextActor>::New();
	auto prop = m_Actor->GetTextProperty();
	prop->SetColor(1, 0, 0);
	prop->SetFontSize(24);
	Helper::SetVtkFont(prop);
	//prop->SetJustificationToCentered();
	//prop->SetVerticalJustificationToCentered();
	
}

void NRender::TextActor2D::SetText(const std::string& txt)
{
	m_Actor->SetInput(txt.c_str());
}

void NRender::TextActor2D::SetPosition(const glm::dvec2& pos)
{
	m_Actor->SetPosition(pos.x, pos.y);
}

void NRender::TextActor2D::SetTextColor(double* rgba)
{
	m_Actor->GetTextProperty()->SetColor(rgba);
	m_Actor->GetTextProperty()->SetOpacity(rgba[3]);
}

std::string NRender::TextActor2D::GetText()
{
	auto input = m_Actor->GetInput();
	return std::string(input);
}

void NRender::TextActor2D::Update(float dt)
{
}

void NRender::TextActor2D::Start()
{
	if (m_RenderEngine && m_Actor)
	{
		m_RenderEngine->GetRenderer()->AddActor2D(m_Actor);
	}
}

void NRender::TextActor2D::Exit()
{
	if (m_RenderEngine && m_Actor)
	{
		m_RenderEngine->GetRenderer()->RemoveActor(m_Actor);
	}
}
