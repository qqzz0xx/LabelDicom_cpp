#pragma once
#include "Header.h"
#include "SceneNode.h"
namespace NRender {
	class MeshActor : public SceneNode
	{
	public:
		MeshActor();
		~MeshActor();

		void SetPickPointEnable(bool v) { m_isPickPointEnable = v; }

		void LoadFromImageData(vtkImageData* imagedata, int val = 200);

		virtual void Update(float dt) override;

	private:
		bool m_isPickPointEnable = false;
	};
}

