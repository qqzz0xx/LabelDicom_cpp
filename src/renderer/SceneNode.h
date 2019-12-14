#pragma once
#include "Header.h"
#include "Behavior.h"
#include "Types.h"

namespace NRender
{
	class RenderEngine;
	class SceneNode : public Behavior, public UObject
	{
	public:
		SceneNode();
		~SceneNode();

		template<class T>
		T* InitActor(T* actor);

		void SetRenderer(RenderEngine* renderer);
		virtual void Update(float dt);
		virtual void Start() override;
		virtual void Exit() override;
		virtual void OnMouseEvent(MouseEventType type) ;
		virtual void OnCameraZoom(double factor) {}
		void ResetCameraToMe();
		RenderEngine* GetRenderEngine() { return m_RenderEngine; }

		std::string GetName();
		void SetName(const std::string& name);
		std::string GetUUID();
		void SetUUID(const std::string& id);

		virtual void SetActive(bool isShow);
		bool GetActive();
		bool GetActiveSelf();
		virtual void SetEnable(bool isEnable);
		virtual bool GetEnable();
		void SetPickable(bool v);

		//Child
		void AddChild(std::shared_ptr<SceneNode> node);
		void RemoveChild(std::shared_ptr<SceneNode> node);
		std::shared_ptr<SceneNode> FindChild(const std::string& name);
		std::shared_ptr<SceneNode> FindChildByID(const std::string& id);
		std::vector<std::shared_ptr<SceneNode>> GetChildList();

		//transfrom
		vtkMatrix4x4* GetWorldMat();
		vtkMatrix4x4* GetLocalMat();
		void SetWorldMat(vtkMatrix4x4* mat);
		void SetLocalMat(vtkMatrix4x4* mat);

		void SetRotation(const glm::dvec3& rotate);
		void SetRotation(const double *rotDegrees);
		void SetPosition(double *p);
		void SetOrigin(const glm::dvec3& origin);

		void SetColor(double* rgb);
		void SetAlpha(double alpha);

		vtkProp3D* GetActor() { return m_Actor; }
		vtkPolyData* GetPolydata();
	protected: 
		virtual void SetActiveSelf(bool isShow);
		void SetActiveParent(bool isShow);
		void SetParent(SceneNode* parent);
		void CheckPickAndMove(vtkProp3D *prop);
		void UpdateMatrix();

	protected:

		vtkProp3D* m_Actor = nullptr;
		RenderEngine* m_RenderEngine = nullptr;

		vtkSmartPointer<vtkMatrix4x4> m_WorldMat;
		vtkSmartPointer<vtkMatrix4x4> m_LocalMat;

		glm::dvec3 rotation;
		glm::dvec3 position;
		glm::dquat quat;

		std::vector<std::shared_ptr<SceneNode>> m_ChildNodes;

		SceneNode* m_ParentNode;

		bool m_IsActiveSelf;
		bool m_IsActive;
		bool m_IsEnable;

	};
	template<class T>
	inline T * SceneNode::InitActor(T * actor)
	{
		m_Actor = actor;
		m_WorldMat = vtkSmartPointer<vtkMatrix4x4>::New();
		m_LocalMat = vtkSmartPointer<vtkMatrix4x4>::New();
		m_Actor->SetUserMatrix(m_WorldMat);

		return actor;
	}
}

