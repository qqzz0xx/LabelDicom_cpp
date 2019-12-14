#pragma once
#include "Header.h"
#include "SceneNode.h"
#include "vtkAlgorithmOutput.h"
#include "vtkPolyDataAlgorithm.h"

class vtkImageReslice;

namespace NRender
{
	class SliceCutterActor : public SceneNode
	{
	public:
		SliceCutterActor();
		~SliceCutterActor();
		template<class T> T* InitSource(T* src);
		template<class T> T* GetSource();

		void SetVoxMat(vtkMatrix4x4* voxMat) { m_VoxMat = voxMat; }
		void SetReslice(vtkImageReslice* reslice) { m_Reslice = reslice; }
		void Refresh();

	private:
		void SetSource(vtkPolyDataAlgorithm* src);

		vtkMatrix4x4* m_VoxMat = nullptr;
		vtkMatrix4x4* m_ResliceMat = nullptr;
		vtkPolyDataAlgorithm* m_Src = nullptr;
		vtkImageReslice* m_Reslice = nullptr;
	};

	template<class T>
	T* NRender::SliceCutterActor::InitSource(T* src)
	{
		SetSource(src);
		return src;
	}

	template<class T>
	inline T* SliceCutterActor::GetSource()
	{
		return (T*)m_Src;
	}

}

