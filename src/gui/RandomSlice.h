#pragma once
#include "Header.h"
#include "ui_RandomSlice.h"
#include "ResliceDelegate.h"

#include "vtkImplicitPlaneRepresentation.h"
#include "vtkPlane.h"

#include <QWidget>

class RandomSlice : public QWidget
{
	Q_OBJECT
public:
	explicit RandomSlice(QWidget* parent = Q_NULLPTR);
	~RandomSlice();
	void Init(vtkImageData* data, MprType type);
	void UpdateResliceByPlane(vtkPlane* plane);
	ResliceDelegate* GetReslice() { return &m_Reslice; }
	Ui_RandomSlice* ui;

private:
	MprType m_Type;
	ResliceDelegate m_Reslice;
	vtkImageData* m_ImageData = nullptr;
	SliceView* m_View;
private:
	void CompResliceMatrix(const glm::dvec3& x, const glm::dvec3& y,
		const glm::dvec3& z, const glm::dvec3& o, vtkMatrix4x4* mat);

protected:
	virtual void showEvent(QShowEvent* event) override;
	virtual void hideEvent(QHideEvent* event) override;

};

