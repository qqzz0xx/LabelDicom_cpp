#pragma once

#include "ui_mainwindow.h"
#include "FrameComponent.h"
#include "MprComponent.h"
#include "LabelEditorDialog.h"
#include "LabelSelectDialog.h"
#include "LabelListComponent.h"
#include "RandomSlice.h"

#include <QMainWindow>
#include <QLabel>
#include <QDialog>
#include <QListWidget>

enum SliceType
{
	SliceType_None,
	SliceType_VoxSlice,
	SliceType_FrameSlice,
};

class LabelDetailView;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = 0);
	~MainWindow();
	SliceType GetSliceType() { return m_SliceType; }
	FrameComponent* GetFrameComponent() { return m_FrameComponent; }
	MprComponent* GetMprComponent() { return m_MprComponent; }
	RandomSlice* GetRandomSlice(MprType type) { return m_RandomSlice[type-MprType::RandomX]; }
	void SetToolsEnable(const std::string& className, bool v);
	void RenderAll();
	int GetCurLabelType();
	std::shared_ptr<ColorLabelItem> GetCurLabelTypeItem();
	bool GetIsInvertPolygon();
	SliceView* GetSliceView(MprType type);
	int* GetCurSliceVal();
	void ForEachRandomSlice(std::function<void(RandomSlice*)> func);

public slots:
	void on_actionLoad_triggered();
	void on_actionImportMask_triggered();
	void on_actionSave_triggered();
	void on_actionLabelEditor_triggered();
	void on_actionClose_triggered();
	void on_actionAnySlice_triggered();
	void on_actionImportJson_triggered();
	void on_actionLabelDetail_triggered();
	void on_actionLoadDir_triggered();
	void on_actionAbout_triggered();

	void on_landmarkButton_clicked(bool v);
	void on_polygonButton_clicked(bool v);
	void on_navLocation_clicked(bool v);
	void on_pushButton_3_clicked(bool v);
	void on_btnRedo_clicked();
	void on_btnUndo_clicked();
	void on_inOpacitySlider_valueChanged(int v);
	void on_rectDelBtn_clicked(bool v);
	void on_rectDrawBtn_clicked(bool v);
	void on_box3DButton_clicked(bool v);
	void on_rectDelBtn_2_clicked(bool v);
	void on_rectDrawBtn_2_clicked(bool v);
	void on_box3DPlusBtn_clicked(bool v);
	void on_filterEdit_textChanged(const QString& txt);

	void OnColorLabelSelected(const QString& text);
	void OnColorLabelParentSelected(const QString& text);

signals:
	void OnToolsClicked(const std::string& className, bool v);

public:
	Ui_MainWindow* ui;
	FrameComponent* m_FrameComponent = nullptr;
	MprComponent* m_MprComponent = nullptr;
	LabelEditorDialog* m_LabelEditorDialog = nullptr;
	LabelSelectDialog* m_LabelSelectDialog = nullptr;
	LabelListComponent* m_LabelListComponent = nullptr;
	RandomSlice* m_RandomSlice[3];
	QDockWidget* m_RandomSliceDock = nullptr;
	QLabel* m_StatusLabel;
	QLabel* m_OpenFileNameLabel;
	SliceType m_SliceType;
	LabelDetailView* m_LabelDetailView = nullptr;
	QListWidget* m_OpenDirWidget = nullptr;

protected:
	int m_SliceVals[4] = { 0 };

	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dropEvent(QDropEvent* event) override;
	void LoadDroppedFile(const QString& file);
	void LoadMainImage(const QString& path, bool encryption);
	//void LoadMainImageAsync(const QString& path, bool encryption);
	void LoadMaskData(const QString& path, bool encryption);
	void LoadDir(const QString& path, const QList<int>& filter );

	void SetEnableRandomSlice(bool v);

	virtual void keyPressEvent(QKeyEvent* event) override;

};
