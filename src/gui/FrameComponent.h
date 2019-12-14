#pragma once
#include "ui_FrameComponent.h"
#include "RenderEngine.h"
#include "SingleImageActor.h"
#include "Interface.h"
#include "LocationActor.h"
#include "Types.h"
#include <QWidget>
#include <QListWidgetItem>

using namespace NRender;

struct ColorLabelItem;

class FrameComponent : public QWidget
{
	Q_OBJECT
public:
	explicit FrameComponent(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	~FrameComponent();

	void Init(vtkImageData* data);
	void OnMouseEvent(MouseEventType ev);
	SliceView* GetSliceView() { return ui->view; }
	int GetCurFrameIndex() { return m_CurIndex; }
	void RefreshIconListColor();

private slots:
	void on_listWidget_currentItemChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void on_nextFrame_clicked();
	void on_preFrame_clicked();
	void on_playFrame_clicked(bool v);
	void on_rePlayFrame_clicked();

	void on_frameLabelButton_clicked();
	void on_clearLabelButton_clicked();

	void on_illnessTypeLabelButton_clicked();
	void on_structLabelButton_clicked();


	void OnLabelTypeChanged(ColorLabelItem* colorLabel);

private:

	Ui_FrameComponent* ui;
	int m_CurIndex = 0;
	int m_PlayInterval = 100; //ms

protected:
	void InitTools();
	void RenderFrame(vtkImageData* data, int index);
	void AddToList(vtkImageData* data, int index);
	QImage VtkImageDataToQImage(vtkImageData* imageData, int radio);
	virtual void resizeEvent(QResizeEvent* event) override;
	void OnZoomChanged(double factor);

};