#pragma once
#include "Header.h"
#include "Types.h"
#include "ResliceActor.h"
#include "RenderEngine.h"
#include "Interface.h"
#include "LocationActor.h"
#include "ResliceDelegate.h"

#include "ui_slicecomponent.h"
#include <QWidget>

using namespace NRender;

class SliceView;

class SliceComponent : public QWidget
{
	Q_OBJECT
public: 
	explicit SliceComponent(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~SliceComponent();
	void Init(vtkImageData* data, MprType type);
	void InitSliderRange(int* dim);
	void RefreshMask();

	void Render();

	MprType GetMprType() { return m_Type; }

	//RenderEngine* GetRenderEngine() { return m_RenderEngine.get(); }

	void SetSliderValueByVoxPos(double* voxPos);

	void SetIgnoreOnceSliceValueChanged();

	SliceView* GetSliceView() { return m_View; }

	ResliceDelegate* GetReslice() { return &m_Reslice; }
	ResliceDelegate* GetMaskReslice() { return &m_MaskReslice; }
	int GetSliderValue() { return ui->horizontalSlider->value(); }
	//const std::shared_ptr<LocationActor>& GetLocationActor() { return m_LocationActor; }

public slots:
	void on_horizontalSlider_valueChanged(int v);
	void on_playButton_clicked(bool v);

protected:
	virtual void resizeEvent(QResizeEvent *event) override;
	void OnMouseEvent(MouseEventType type);

public:
	Ui_SliceComponent *ui;
	//std::shared_ptr<LocationActor> m_LocationActor;
	//std::shared_ptr<ResliceActor> m_ResliceActor;
	std::function<void(SliceComponent*, MouseEventType)> OnMouseEventCallback = nullptr;
	std::function<void(SliceComponent*, int v)> OnSliceValueChangedCallback = nullptr;

protected:
	MprType m_Type;
	ResliceDelegate m_Reslice;
	ResliceDelegate m_MaskReslice;

	vtkImageData* m_ImageData = nullptr;
	SliceView* m_View;
	//std::shared_ptr<RenderEngine> m_RenderEngine;
	bool m_IgnoreOnceSliceValueChanged = false;
};