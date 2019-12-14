#pragma once
#include "Header.h"
#include "Behavior.h"
#include "NWindowInteractor.h"
#include "NSliceInteractorStyle.h"
#include "SceneManager.h"
#include "StepTimer.h"

namespace NRender
{
	class RenderEngine : public Behavior
	{

	public:
		RenderEngine();
		~RenderEngine();

		void SetParallelProjection(bool v);

		void SetRenWinSize(int width, int height);
		void SetRenWinParentId(void* ptr);
		// Í¨¹ý Behavior ¼Ì³Ð
		virtual void Start() override;

		virtual void Update(float dt) override;

		virtual void Exit() override;

		void Render();

		int* GetEventPos();

		double* GetEventWorldPos();

		SceneManager* GetSceneManager() { return m_SceneManager.get(); }
		vtkRenderer* GetRenderer() { return m_Renderer; }
		NSliceInteractorStyle* GetInteractorStyle() { return m_InteractorStyle; }
		NWindowInteractor* GetInteractor() { return m_Interactor; }

		std::function<void(MouseEventType)> OnMouseEventCallback = nullptr;

	private:
		void OnMouseEvent(MouseEventType type);
		void InitListener();

	private:
		vtkSmartPointer<vtkRenderer> m_Renderer;
		vtkSmartPointer<vtkRenderWindow> m_Renwin;
		vtkSmartPointer<NWindowInteractor> m_Interactor;
		vtkSmartPointer<NSliceInteractorStyle> m_InteractorStyle;

		//vtkInteractorStyle* m_InteractorStyle;

		StepTimer m_Timer;
		std::shared_ptr<SceneManager> m_SceneManager;

		double m_EventWorldPos[3];

	};
}