#pragma once
#include "Header.h"
#include "SceneNode.h"
class vtkCubeSource;

namespace NRender {
	class CubeActor : public SceneNode
	{
	public:
		CubeActor();
		~CubeActor();

		virtual void Update(float dt) override;

		void SetPosition(double x, double y, double z);
		void SetColor(double r, double g, double b);
		void SetRGBA(double* rgba);
		void SetXYZLength(double x, double y, double z);
	private:
		long m_CmdTag;
		vtkSmartPointer<vtkCubeSource> m_CubeSource;
	};
}