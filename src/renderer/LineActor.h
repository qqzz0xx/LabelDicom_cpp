#pragma once
#include "Header.h"
#include "SceneNode.h"

#include "vtkLineSource.h"
#include "vtkTextActor3D.h"

namespace NRender
{
	class LineActor :public SceneNode
	{
	public:
		LineActor();
		~LineActor();

		void SetPos1(double* p1);
		void SetPos2(double* p2);
		void SetLineWidth(double w);
		void UpdateDist();
		virtual void SetActive(bool isShow);

		void SetEnableDist(bool v);

		double GetLength();
		void SetLength(double len);
		void Refresh();
	private:
		vtkSmartPointer<vtkLineSource> m_LineSrc;
		double m_Length = 100;
		
		vtkSmartPointer<vtkTextActor3D> m_TextActor;
	};

}
