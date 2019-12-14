#include "LocationTools.h"
#include "SliceComponent.h"
#include "CoordHelper.h"
#include "MprComponent.h"
#include "FrameComponent.h"
#include "AppMain.h"


void LocationTools::OnMouseEvent(MouseEventType type)
{
	if (type == MouseEventType_LeftDown)
	{
		UpdateLocation();
	}
	if (type == MouseEventType_Move)
	{
		if (m_View->GetRenderEngine()->GetInteractorStyle()->LeftButtonPress)
		{
			UpdateLocation();
		}
	}
}

void LocationTools::SetEnable(bool v)
{
	__super::SetEnable(v);
	m_View->GetLocationActor()->SetActive(v);
}


void LocationTools::UpdateLocation()
{

	auto actor = m_View->GetLocationActor();
	if (actor->GetEnable())
	{
		auto p = m_View->GetRenderEngine()->GetEventWorldPos();
		auto spc = m_View->GetSingleImageActor()->GetImageData()->GetSpacing();

		p[0] = std::round(p[0] / spc[0]) * spc[0];
		p[1] = std::round(p[1] / spc[1]) * spc[1];
		p[2] = 0;
		
		auto old = actor->GetActor()->GetPosition();
		if (old[0] == p[0] && old[1] == p[1])
		{
			return;
		}

		actor->SetPosition(p);
		m_View->GetRenderEngine()->Render();

		if (LocationCallback != nullptr)
		{
			double xy[3] = { 0 };

			CoordHelper::WorldToImageXY(p, actor, xy);
			LocationCallback(m_View, xy);
		}
	}
}

//void LocationTools::OnSliderValueChange(MprComponent * mpr, SliceComponent * slice, int value)
//{
//	auto pos = slice->m_LocationActor->GetActor()->GetPosition();
//	double vpos[3];
//	CoordHelper::WorldToVoxPos(pos, slice->m_ResliceActor.get(), vpos);
//
//	UpdateMprByVoxPos(mpr, slice, vpos);
//}

void LocationTools::UpdateMprByVoxPos(MprComponent * mpr, SliceComponent* slice, double* vpos)
{
	//auto type = slice->GetMprType();
	//for (size_t i = 0; i < 3; i++)
	//{
	//	auto ss = mpr->mpr[i];
	//	if (i != type)
	//	{
	//		ss->SetIgnoreOnceSliceValueChanged();
	//		ss->SetSliderValueByVoxPos(vpos);
	//		double tpos[3] = {};
	//		CoordHelper::VoxToWorldPos(vpos, ss->m_ResliceActor.get(), tpos);
	//		//LOG(INFO) << tpos[0] << " " << tpos[1] << " " << tpos[2];
	//		tpos[2] = 0;
	//		ss->m_LocationActor->SetPosition(tpos);
	//	}
	//}

	//mpr->Render();
}
