#pragma once
#include "Header.h"
#include "RenderEngine.h"
#include "VolumeActor.h"
#include "MeshActor.h"
#include "LocationActor3D.h"
#include "vtkImplicitPlaneWidget2.h"

#include "ui_VolumeComponent.h"
#include <QWidget>

using namespace NRender;
struct ColorLabelItem;
namespace NRender {
	class BoxWidgetActor;
};

class VolumeComponent : public QWidget
{
	Q_OBJECT
public:
	explicit VolumeComponent(QWidget* parent = Q_NULLPTR, 
		Qt::WindowFlags f = Qt::WindowFlags());
	~VolumeComponent();

	void Init(vtkImageData* data);
	void InitPlaneWidget(vtkImageData* data);
	void InitLocation3D(vtkImageData* data);
	void SetLocation3DPos(double* vpos);
	RenderEngine* GetRenderEngine() { return m_RenderEngine.get(); }
	void Render();
	VolumeActor* GetVolumeActor() { return m_VolumeActor.get(); }
	vtkImplicitPlaneWidget2* GetPlaneWidget() { return m_PlaneWidget; }

	Ui_VolumeComponent* ui;

	void OnLabelTypeChanged(ColorLabelItem* labelType);

private slots:
	void on_loadVolume_clicked();
	void on_updateMesh_clicked();
	void on_showMesh_clicked(bool v);
	void on_showVolume_clicked(bool v);
	void on_showPlaneWidget_clicked(bool v);

private:
	std::shared_ptr<RenderEngine> m_RenderEngine;
	std::shared_ptr<VolumeActor> m_VolumeActor;
	LocationActor3D* m_LocationActor3D = nullptr;
	vtkSmartPointer<vtkImplicitPlaneWidget2> m_PlaneWidget;
	std::weak_ptr<NRender::BoxWidgetActor> m_VolumeBoundsBox;
	//std::vector<std::weak_ptr<MeshActor>> m_Meshes;
protected:
	virtual void resizeEvent(QResizeEvent* event) override;

};

