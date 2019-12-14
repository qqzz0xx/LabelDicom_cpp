#include "SliceView.h"
#include "AppMain.h"
#include "CoordHelper.h"
#include "MprPositionModel.h"
#include "PolygonTools.h"
#include "WinLevelWidthTools.h"
#include "CameraZoomAndMoveTools.h"
#include "RandomSliceBoxTools.h"
#include "LandMarkListModel.h"
#include "Helper.h"
#include "GlobalVal.h"

#include "vtkImageActor.h"
#include "vtkImageProperty.h"

#include "valsalva_class.h"

#include <QDialog>

class CameraZoomAndMoveTools;
class WinLevelWidthTools;
class LocationTools;
class PolygonTools;
class RandomSliceBoxTools;

SliceView::SliceView(QWidget* parent) :
	QWidget(parent),
	ui(new Ui_SliceView)
{
	ui->setupUi(this);

	m_RenderEngine = std::make_shared<RenderEngine>();
	m_RenderEngine->OnMouseEventCallback = [&](auto ev) {OnMouseEvent(ev); };

	int width = ui->label->size().width();
	int height = ui->label->size().height();
	m_RenderEngine->SetRenWinSize(width, height);
	m_RenderEngine->SetRenWinParentId((void*)ui->label->winId());
	m_RenderEngine->Start();



	connect(gMainWindow, &MainWindow::OnToolsClicked, this, &SliceView::OnToolsEnable);
}

SliceView::~SliceView()
{
	m_RenderEngine->Exit();
}

void SliceView::Init(vtkImageData* data, MprType type)
{
	m_Type = type;


	if (m_SingleImageActor == nullptr)
	{
		auto p = std::make_shared<SingleImageActor>();
		p->Init(data);
		m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(p);
		m_SingleImageActor = p.get();

	}

	if (m_MaskImageActor == nullptr)
	{
		auto p = std::make_shared<SingleImageActor>();
		m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(p);
		p->SetActive(false);
		auto actor = vtkImageActor::SafeDownCast(p->GetActor());
		actor->GetProperty()->SetOpacity(0.6);

		m_MaskImageActor = p.get();
	}

	if (m_LocationActor == nullptr)
	{
		auto p = std::make_shared<LocationActor>();
		m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(p);
		p->SetColorOfType(type);

		m_LocationActor = p.get();
	}

	//if (m_LabelDescActor == nullptr)
	//{
	//	auto p = std::make_shared<TextActor2D>();
	//	m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(p);
	//	p->SetPosition(glm::dvec2(0, 0));
	//	p->SetText("");
	//	m_LabelDescActor = p.get();
	//}

	int width = ui->label->size().width() - 200;
	int height = ui->label->size().height() - 50;

	glm::dvec2 descPos[4] = {
		{0,0}, {width, 0}, {0, height}, {width,height}
	};

	for (size_t i = 0; i < 4; i++)
	{
		auto p = std::make_shared<TextActor2D>();
		m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(p);
		p->SetPosition(descPos[i]);
		p->SetText("");
		m_LabelDescActors[i] = p.get();
	}

	m_SingleImageActor->ResetCamera();
	m_RenderEngine->Render();

	m_InitialCameraZoom = m_RenderEngine->GetRenderer()->GetActiveCamera()->GetParallelScale();

	ui->stackedWidget->setCurrentWidget(ui->pageNone);

}

void SliceView::Render(vtkImageData* data)
{
	if (m_SingleImageActor && m_RenderEngine)
	{
		m_SingleImageActor->SetImageData(data);
		m_SingleImageActor->Refresh();

		m_RenderEngine->Render();
	}
}

void SliceView::Render()
{
	if (m_RenderEngine)
	{
		m_RenderEngine->Render();
	}
}

void SliceView::OnToolsEnable(const std::string& name, bool v)
{
	if (name == typeid(LocationTools*).name())
	{
		m_Tools.ForEach([&](BaseSliceTools* t) {
			t->SetEnable(false);
		});
		auto func = [](auto tools)
		{
			if (tools) tools->SetEnable(true);
		};
		func(m_Tools.Get<WinLevelWidthTools>());
		func(m_Tools.Get<CameraZoomAndMoveTools>());
	}
	else
	{
		m_Tools.ForEach([&](const std::string& name, BaseSliceTools* t) {

			if (name != typeid(CameraZoomAndMoveTools*).name()
				&& name != typeid(RandomSliceBoxTools*).name()
				)
			{
				t->SetEnable(false);
			}
		});
	}

	m_Tools.DoIf(name, [v](BaseSliceTools* t) {
		t->SetEnable(v);
	});

	m_RenderEngine->Render();
}

void SliceView::SetLabelDesc(const std::string& desc, int idx)
{
	if (m_LabelDescActors[idx])
	{
		m_LabelDescActors[idx]->SetText(desc);
	}
}

void SliceView::SetLabelDescCN(const char* desc, int idx)
{
	if (m_LabelDescActors[idx])
	{
		m_LabelDescActors[idx]->m_Actor->SetInput(desc);
	}
}

void SliceView::SetLabelDescColor(double* rgb, int idx)
{
	if (m_LabelDescActors[idx])
	{
		m_LabelDescActors[idx]->SetTextColor(rgb);
	}
}

std::string SliceView::GetLabelDesc(int idx)
{
	if (m_LabelDescActors[idx])
	{
		return m_LabelDescActors[idx]->GetText();
	}

	return "";
}

void SliceView::on_clearDraw_clicked()
{
	auto t = m_Tools.Get<PolygonTools>();
	t->OnClear();
}

void SliceView::on_complete_clicked()
{
	auto t = m_Tools.Get<PolygonTools>();
	t->OnComplete();
}

void SliceView::on_paste_clicked()
{
	auto t = m_Tools.Get<PolygonTools>();
	t->OnPaste();
}

void SliceView::on_accpet_clicked()
{
	auto t = m_Tools.Get<PolygonTools>();
	t->OnAccpet();
}

void SliceView::on_clearPolygon_clicked()
{
	auto t = m_Tools.Get<PolygonTools>();
	t->OnClear();
}

void SliceView::on_split_clicked()
{
	auto t = m_Tools.Get<PolygonTools>();
	t->OnSplit();
}

void SliceView::on_del_clicked()
{
	auto t = m_Tools.Get<PolygonTools>();
	t->OnDelete();
}

void SliceView::on_accpetBox_clicked()
{
	auto t = m_Tools.Get<RandomSliceBoxTools>();
	if (t) t->OnAccpet();
}

void SliceView::on_xPlaneButton_clicked()
{
	auto t = m_Tools.Get<RandomSliceBoxTools>();
	if (t) t->OnRotatePlanetToX();
}

void SliceView::on_yPlaneButton_clicked()
{
	auto t = m_Tools.Get<RandomSliceBoxTools>();
	if (t) t->OnRotatePlanetToY();
}

void SliceView::on_zPlaneButton_clicked()
{
	auto t = m_Tools.Get<RandomSliceBoxTools>();
	if (t) t->OnRotatePlanetToZ();
}

void SliceView::on_pasteLandMark_clicked()
{
	auto t = gModelMgr->GetModel<LandMarkListModel>();
	if (t) t->DoPaste(this);
}

void SliceView::on_clearBox3D_clicked()
{
	auto t = m_Tools.Get<RandomSliceBoxTools>();
	if (t) t->OnClear();
}

void SliceView::on_ploySegButton_clicked()
{
	auto dialog = new QDialog(this);
	auto layout = new QVBoxLayout(dialog);

	for (auto&& v : g_ConfigVal.SegModelPaths)
	{
		auto btn = new QPushButton(QString::fromStdString(v.first), dialog);
		layout->addWidget(btn);
		connect(btn, &QPushButton::clicked, [&, dialog, path = v.second]{
			auto t = m_Tools.Get<PolygonTools>();
			t->OnAutoSeg(path);
			dialog->accept();
		});
	}

	dialog->exec();
	delete dialog;
	
}

void SliceView::OnMouseEvent(MouseEventType ev)
{
	auto model = gModelMgr->GetModel<MprPositionModel>();
	auto pos = m_RenderEngine->GetEventPos();
	for (size_t i = 0; i < 2; i++)
	{
		model->m_SceenPos[i] = pos[i];
	}
	CoordHelper::SceenToWorldPos(pos, m_RenderEngine->GetRenderer(), model->m_WorldPos);
	CoordHelper::WorldToImageXY(model->m_WorldPos, m_SingleImageActor, model->m_ImageXY);
	model->m_Pixel = m_SingleImageActor->GetPixel(model->m_ImageXY[0], model->m_ImageXY[1]);

	if (OnMouseEventCallback != nullptr)
	{
		OnMouseEventCallback(this, ev);
	}

	m_Tools.ForEach([&](BaseSliceTools* tool) {
		if (tool->GetEnable())
		{
			tool->OnMouseEvent(ev);
		}
	});

	gMainWindow->m_StatusLabel->setText(model->ToQtString());

	if (ev == MouseEventType_KeyPress)
	{
		auto it = m_RenderEngine->GetInteractor();
		auto keyCode = std::string(it->GetKeySym());
		auto isControl = it->GetControlKey();
		if (keyCode == "W" || keyCode == "w")
		{
			if (isControl)
			{
				gMainWindow->on_actionClose_triggered();
			}
		}
		else if (keyCode == "F1")
		{
			auto reply = gMainWindow->m_LabelSelectDialog->exec();
			if (reply == QDialog::Accepted)
			{
				auto label = gMainWindow->m_LabelSelectDialog->value();
				auto index = gMainWindow->ui->comboBox->findText(label->ID);
				if (index != -1) { // -1 for not found
					gMainWindow->ui->comboBox->setCurrentIndex(index);
				}
			}
		}
	}
}

void SliceView::resizeEvent(QResizeEvent* event)
{
	if (m_RenderEngine)
	{
		int width = ui->label->size().width();
		int height = ui->label->size().height();
		m_RenderEngine->SetRenWinSize(width, height);

		int w = width - 200;
		int h = height - 50;
		glm::dvec2 descPos[4] = {
			{0,0}, {w, 0}, {0, h}, {w,h}
		};

		for (size_t i = 0; i < 4; i++)
		{
			auto p = m_LabelDescActors[i];
			if (p)
			{
				p->SetPosition(descPos[i]);
			}
		}
	}
}
