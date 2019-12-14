#pragma once
#include "Header.h"
#include "Interface.h"
#include "Table.h"
#include "BaseMprTools.h"
#include "ui_MprComponent.h"
#include <QWidget>

class SliceComponent;
struct ColorLabelItem;

class MprComponent : public QWidget
{
	Q_OBJECT
public:
	explicit MprComponent(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	void Init(vtkImageData* data);
	void OnMouseEvent(SliceComponent* slice, MouseEventType type);
	void OnSliderValueChanged(SliceComponent* slice, int value);
	void RefreshMask();
	void Render();
	VolumeComponent* GetVolumeComponent() { return ui->widget_4; }
	void ForEach(std::function<void(SliceComponent*)> func);
	SliceComponent* GetSliceComponent(MprType type);
	int* GetCurSliceVal();
	
public:
	Ui_MprComponent* ui;
	SliceComponent* mpr[3];
	bool m_bInit = false;

private:
	void InitTools();
	void OnLocationCallback(SliceComponent* sc, double* imgXY);
	void OnLabelTypeChanged(ColorLabelItem* colorLabel);
	void OnZoomChanged(SliceComponent* sc, double factor);
	void OnSliceFullClicked(MprType type, bool v);
	int m_SliceVals[3] = { 0 };
	//Table<IMprEventListener> m_MprEventListeners;
	

};