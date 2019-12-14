#include "LocationActor3D.h"
#include "AppMain.h"
#include "LoadMgr.h"

NRender::LocationActor3D::LocationActor3D()
{
	m_Name = "LocationActor3D";

	for (size_t i = 0; i < 3; i++)
	{
		auto p = std::make_shared<LineActor>();
		double rgb[3] = { 0,0,0 };
		rgb[i] = 1;
		p->SetColor(rgb);
		AddChild(p);
		m_Lines.emplace_back(p);
	}
}

void NRender::LocationActor3D::SetVoxDim(int* dim)
{
	for (size_t i = 0; i < 3; i++)
	{
		m_Dim[i] = dim[i];
	}
	auto spc = gLoadMgr->GetImageData()->GetSpacing();
	double center[3];
	for (size_t i = 0; i < 3; i++)
	{
		center[i] = (int)((dim[i]*spc[i] - 1) / 2.0 );
	}

	SetVoxPos(center);
}

void NRender::LocationActor3D::SetVoxPos(double* vpos)
{
	auto spc = gLoadMgr->GetImageData()->GetSpacing();
	{
		double p1[3] = { 0, vpos[1], vpos[2] };
		double p2[3] = { m_Dim[0] * spc[0] - 1, vpos[1], vpos[2] };
		m_Lines[0]->SetPos1(p1);
		m_Lines[0]->SetPos2(p2);
	}
	{
		double p1[3] = { vpos[0], 0, vpos[2] };
		double p2[3] = { vpos[0], m_Dim[1] * spc[1] - 1, vpos[2] };
		m_Lines[1]->SetPos1(p1);
		m_Lines[1]->SetPos2(p2);
	}
	{
		double p1[3] = { vpos[0], vpos[1], 0 };
		double p2[3] = { vpos[0], vpos[1], m_Dim[2] * spc[2] - 1 };
		m_Lines[2]->SetPos1(p1);
		m_Lines[2]->SetPos2(p2);
	}
}
