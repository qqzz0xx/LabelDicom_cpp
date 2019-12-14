#pragma once
#include "Header.h"
#include "BaseSliceTools.h"
#include "PolygonActor.h"
#include "RectangleActor.h"
#include "Structs.h"
using namespace NRender;

class PolygonTools : public BaseSliceTools
{
public:
	enum State
	{
		Start,
		AddPoint,
		PickPoint,
		DragPoint,
		EditorBox,
		MoveBox,
		End,
	};
	enum Mode
	{
		Curve,
		SelectPoint,
	};

	PolygonTools();

	virtual void OnMouseEvent(MouseEventType type) override;
	virtual void SetEnable(bool v) override;
	void OnComplete();
	void OnPaste();
	void OnClear();
	void OnAccpet();
	void OnDelete();
	void OnSplit();
	void OnAutoSeg(const std::string& path);

private:
	bool CheckClose(const glm::dvec3& p);
	std::vector<int> CheckPick(const glm::dvec3& p);
	bool CheckSelect();
	void ResetAllPoints();
	void UpateBox();
	void UpdateDraw();
	void SwitchState(State state);
	void SaveLastPoint();
	void AddOnePoint(const glm::dvec3& p);
	int GetCurLabelType();

private:
	std::shared_ptr<PolygonActor> m_Actor;
	std::shared_ptr<PointActor> m_CurPickPoint;
	std::shared_ptr<RectangleActor> m_RectangleActor;

	Mode m_Mode = Curve;
	bool m_IsSaveLast = false;
	bool m_IsComplete = false;
	double m_PickedDist = 5;
	double m_CloseDist = 5;
	double m_AddPointDragDist = 15;
	glm::dvec3 m_BoxOffset = glm::dvec3(5, 5, 0);


	std::vector<PolygonPoint> m_Points;
	std::vector<PolygonPoint> m_LastPoints;
	int m_CurPickIdx = -1;
	State m_State = State::Start;
	glm::dvec3 m_SelcetBox[2];
	glm::dvec3 m_EventStartPos;
	glm::dvec3 m_RectStartPos;

	double m_SelectColor[3] = { 1,1,0 };
	double m_DefualtColor[3] = { 1, 0.9, 0.6 };
};

