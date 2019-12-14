#include "BaseSliceTools.h"
#include "SliceView.h"

BaseSliceTools::BaseSliceTools()
{
}


BaseSliceTools::~BaseSliceTools()
{
}

void BaseSliceTools::OnMouseEvent(MouseEventType type)
{
}

void BaseSliceTools::SetCurIndex(int index)
{
	m_CurIndex = index;
}

bool BaseSliceTools::IsLeftDrag(MouseEventType type)
{
	auto istyle = m_View->GetRenderEngine()->GetInteractorStyle();
	if (type == MouseEventType_Move)
	{
		if (istyle->LeftButtonPress)
		{
			return true;
		}
	}

	return false;
}

bool BaseSliceTools::IsRightDrag(MouseEventType type)
{
	auto istyle = m_View->GetRenderEngine()->GetInteractorStyle();
	if (type == MouseEventType_Move)
	{
		if (istyle->RightButtonPress)
		{
			return true;
		}
	}

	return false;
}

bool BaseSliceTools::IsMiddleDrag(MouseEventType type)
{
	auto istyle = m_View->GetRenderEngine()->GetInteractorStyle();
	if (type == MouseEventType_Move)
	{
		if (istyle->MiddleButtonPress)
		{
			return true;
		}
	}

	return false;
}
