#pragma once

#include "Header.h"
#include "ModelBase.h"
#include "RenderEngine.h"
#include "MeshActor.h"

using namespace NRender;

class MeshModel : public ModelBase
{
public:
	MeshModel();

	void SetRenderEngine(RenderEngine* re) { m_RenderEngine = re; }
	void UpdateAll();
	void ShowAll(bool v);
	void UpdateColor(int labalType, const QColor& color);

private:
	std::map<int, std::string> m_Meshes;
	RenderEngine* m_RenderEngine = nullptr;
};

