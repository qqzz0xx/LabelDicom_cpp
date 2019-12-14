#include "ActorBase.h"

void ActorBase::Init(vtkRenderer * renderer)
{
	m_Renderer = renderer;
}

void ActorBase::SetActive(bool v)
{
	if (m_Renderer&&m_Actor)
	{
		if (v)
		{
			m_Renderer->AddActor(m_Actor);
		}
		else
		{
			m_Renderer->RemoveActor(m_Actor);
		}
	}
}
