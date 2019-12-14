#pragma once
#include "Header.h"

class ActorBase : public UObject
{
public:
	virtual void Init(vtkRenderer* renderer);
	virtual void SetActive(bool v);
	vtkProp3D* GetActor() { return m_Actor; }
protected:
	vtkProp3D* m_Actor = nullptr;
	vtkRenderer* m_Renderer = nullptr;
};

