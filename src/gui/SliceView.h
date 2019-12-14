#pragma once
#include <QWidget>
#include "RenderEngine.h"
#include "SingleImageActor.h"
#include "Interface.h"
#include "LocationActor.h"
#include "Types.h"
#include "Table.h"
#include "BaseSliceTools.h"
#include "TextActor2D.h"

#include "ui_SliceView.h"


using namespace NRender;

class SliceView : public QWidget
{
	Q_OBJECT
public:
	explicit SliceView(QWidget* parent = Q_NULLPTR);
	~SliceView();

	void Init(vtkImageData* data, MprType type);
	void Render(vtkImageData* data);
	void Render();
	void OnToolsEnable(const std::string& name, bool v);
	//void SetPolygonOpPage(int idx);
	double GetInitialCameraZoom() { return m_InitialCameraZoom; }
	void SetLabelDesc(const std::string& desc, int idx);
	void SetLabelDescCN(const char* desc, int idx);
	void SetLabelDescColor(double* rgb, int idx);
	std::string GetLabelDesc(int idx);

	Table<BaseSliceTools>* GetTools() { return &m_Tools; }

	LocationActor* GetLocationActor() { return m_LocationActor; }
	RenderEngine* GetRenderEngine() { return m_RenderEngine.get(); }
	SingleImageActor* GetSingleImageActor() { return m_SingleImageActor; }
	SingleImageActor* GetMaskImageActor() { return m_MaskImageActor; }
	MprType GetMprType() { return m_Type; }

	std::function<void(SliceView*, MouseEventType)> OnMouseEventCallback = nullptr;
	Ui_SliceView* ui;

private slots:
	void on_clearDraw_clicked();
	void on_complete_clicked();
	void on_paste_clicked();
	void on_accpet_clicked();
	void on_clearPolygon_clicked();
	void on_split_clicked();
	void on_del_clicked();
	void on_accpetBox_clicked();
	void on_xPlaneButton_clicked();
	void on_yPlaneButton_clicked();
	void on_zPlaneButton_clicked();
	void on_pasteLandMark_clicked();
	void on_clearBox3D_clicked();
	void on_ploySegButton_clicked();

protected:
	void OnMouseEvent(MouseEventType ev);
	virtual void resizeEvent(QResizeEvent* event) override;

private:


	SingleImageActor* m_SingleImageActor = nullptr;
	SingleImageActor* m_MaskImageActor = nullptr;
	TextActor2D* m_LabelDescActor = nullptr;
	TextActor2D* m_LabelDescActors[4] = {};

	LocationActor* m_LocationActor = nullptr;
	std::shared_ptr<RenderEngine> m_RenderEngine;
	MprType m_Type;

	double m_InitialCameraZoom;

	Table<BaseSliceTools> m_Tools;
};

