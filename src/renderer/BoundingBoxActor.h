#pragma once
#include "Header.h"
#include "SceneNode.h"
#include "RectangleActor.h"
class vtkPlaneSource;
class vtkTextActor3D;
struct BoundingBoxLabelItem;
namespace NRender
{
	class BoundingBoxActor : public RectangleActor
	{
	public:
		BoundingBoxActor();
		void Init(std::shared_ptr<BoundingBoxLabelItem> item);
		virtual void Start() override;
		virtual void Exit() override;
		virtual void Update(float dt) override;
		void UpdateColor();

		virtual void SetPoints(const glm::dvec3& p1, const glm::dvec3& p2) override;
		virtual void OnMouseEvent(MouseEventType type) override;
		bool CheckHitBox(const glm::dvec3& pos);

		virtual void SetEnable(bool isEnable) override;
		virtual void SetActive(bool isShow) override;

	protected:
	
		int CheckPick(const glm::dvec3& p);
		void SetBounds(const glm::dvec3& p1, const glm::dvec3& p2);
		vtkSmartPointer<vtkActor> m_BoundingActors[8];
		vtkSmartPointer<vtkActor> m_PlaneActor;
		vtkSmartPointer<vtkPlaneSource> m_PlaneSrc;
		glm::dvec3 m_Bounds[8];

		enum State
		{
			None,
			Move,
			DragPoint,
		};
		State m_State = None;
		int m_DragPointIdx;
		glm::dvec3 m_StartPos;
		glm::dvec3 m_DragBoxPos[2];
		std::shared_ptr<BoundingBoxLabelItem> m_Model;
		
		vtkSmartPointer<vtkTextActor3D> m_TextActor;
	};
}

