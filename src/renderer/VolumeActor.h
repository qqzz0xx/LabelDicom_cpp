#pragma once
#include "Header.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "SceneNode.h"

namespace NRender
{

	class ResliceActor;

	class VolumeActor : public SceneNode
	{
	public:
		VolumeActor();
		~VolumeActor();

		virtual void Start() override;

		void LoadFromImageData(vtkImageData* imagedata);

		void SetColorTable(std::string fileName);

		void MoveColorTable(double delta);

		vtkImageData* GetImageData();
		vtkGPUVolumeRayCastMapper* GetMapper();

		virtual void Update(float dt) override;

		void InitImageData(vtkImageData* data);

		void InitReslice(vtkImageData* data);

		void UpdateRelice(MprType type, int v);
		void UpdateRelice(MprType type, vtkMatrix4x4* mat);
		void UpdateResliceLevelWindow(MprType type, double level, double window);

		void ShowReslice(MprType type, bool isShow);

		void SetReliceAlpha(double alpha);

		void ShowVolume(bool v);

		vtkVolume* m_Volume;



	private:

	private:
		vtkSmartPointer<vtkImageData> m_VolumeData = nullptr;
		std::string m_CurColorTableName;
		double m_WindowLevel, m_WindowWidth;


		std::vector<std::shared_ptr<ResliceActor>> m_ResliceNodeList;
	};
}