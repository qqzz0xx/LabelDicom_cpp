#pragma once

#include "SceneNode.h"
#include "LineActor.h"

namespace NRender
{
	class LocationActor3D : public SceneNode
	{
	public:
		LocationActor3D();
		void SetVoxDim(int* dim);
		void SetVoxPos(double* vpos);

	private:
		std::vector<std::shared_ptr<LineActor>> m_Lines;
		int m_Dim[3];
	};
}
