#include "LocationActor.h"
#include "RenderEngine.h"
#include "AppMain.h"
#include "MprPositionModel.h"

NRender::LocationActor::LocationActor()
{
	m_Name = "LocationActor";

	double zpos = 0;

	double len = 4 * 1000;
	double xp1[3] = { -len, 0, zpos };
	double xp2[3] = { len, 0, zpos };
	m_AxisX.SetPos1(xp1);
	m_AxisX.SetPos2(xp2);
	double yp1[3] = { 0, -len, zpos };
	double yp2[3] = { 0, len, zpos };
	m_AxisY.SetPos1(yp1);
	m_AxisY.SetPos2(yp2);

	m_AxisX.SetAlpha(0.8);
	m_AxisY.SetAlpha(0.8);

	auto actor = InitActor(vtkAssembly::New());
	actor->AddPart(m_AxisX.GetActor());
	actor->AddPart(m_AxisY.GetActor());
}

void NRender::LocationActor::SetXAxisColor(double* rgb)
{
	m_AxisX.SetColor(rgb);
}

void NRender::LocationActor::SetYAxisColor(double* rgb)
{
	m_AxisY.SetColor(rgb);
}

void NRender::LocationActor::SetColorOfType(MprType type)
{
	double red[3] = { 1, 0, 0 };
	double green[3] = { 0, 1, 0 };
	double blue[3] = { 0, 0, 1 };

	if (type == MprType::Axial)
	{
		SetXAxisColor(red);
		SetYAxisColor(green);
	}
	else if (type == MprType::Coronal)
	{
		SetXAxisColor(red);
		SetYAxisColor(blue);
	}
	else if (type == MprType::Sagittal)
	{
		SetXAxisColor(green);
		SetYAxisColor(blue);
	}
	else
	{
		SetXAxisColor(red);
		SetYAxisColor(green);
	}
}

void NRender::LocationActor::OnMouseEvent(MouseEventType type)
{
	/*
	if (type == MouseEventType_LeftDown)
	{
		auto pos = m_RenderEngine->GetEventWorldPos();

		auto model = gModelMgr->GetModel<MprPositionModel>();

		SetPosition(pos);

		//auto imageActor = m_RenderEngine->m_SceneManager->FindNode("SingleImageActor");
		//auto mat = imageActor->GetWorldMat();

		m_RenderEngine->Render();
	}
	*/
}
