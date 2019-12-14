#include "VolumeComponent.h"
#include "AppMain.h"
#include "LoadMgr.h"
#include "ModelManager.h"
#include "MeshModel.h"
#include "NCommand.h"
#include "BoxWidgetActor.h"

#include "vtkImplicitPlaneRepresentation.h"
#include "vtkPlane.h"

#include <QDir>
#include <QResizeEvent>

using namespace NRender;

VolumeComponent::VolumeComponent(QWidget* parent /*= Q_NULLPTR*/,
	Qt::WindowFlags f /*= Qt::WindowFlags()*/) :
	QWidget(parent),
	ui(new Ui_VolumeComponent)
{
	ui->setupUi(this);
	ui->showPlaneWidget->setChecked(true);

	m_RenderEngine = std::make_shared<RenderEngine>();

	m_RenderEngine->SetParallelProjection(false);
	auto size = ui->label->size();
	m_RenderEngine->SetRenWinSize(size.width(), size.height());
	m_RenderEngine->SetRenWinParentId((void*)ui->label->winId());
	m_RenderEngine->Start();

	auto istyle = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	m_RenderEngine->GetInteractor()->SetInteractorStyle(istyle);

	m_RenderEngine->Render();

	auto model = gModelMgr->GetModel<MeshModel>();
	model->SetRenderEngine(m_RenderEngine.get());

	m_PlaneWidget = vtkSmartPointer<vtkImplicitPlaneWidget2>::New();

	connect(gMainWindow->ui->voxOpacitySlider, &QSlider::valueChanged, [&](auto v) {

		if (m_VolumeActor)
		{
			m_VolumeActor->MoveColorTable(v);

			Render();
		}
	});
}

VolumeComponent::~VolumeComponent()
{
	m_VolumeActor.reset();
	m_RenderEngine.reset();
}

void VolumeComponent::Init(vtkImageData* data)
{
	if (m_VolumeActor == nullptr)
	{
		m_VolumeActor = std::make_shared<VolumeActor>();
		m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(m_VolumeActor);


	}

	m_VolumeActor->LoadFromImageData(data);
	auto root = QDir::currentPath();
	auto path = root + "/lut_tabel/1.xml";
	m_VolumeActor->SetColorTable(path.toStdString());

	m_VolumeActor->SetActive(ui->showVolume->isChecked());

	//m_PlaneWidget->AddObserver(vtkCommand::InteractionEvent, myCallback);

	m_RenderEngine->Render();
}

void VolumeComponent::InitPlaneWidget(vtkImageData* data)
{
	vtkSmartPointer<vtkImplicitPlaneRepresentation> rep =
		vtkSmartPointer<vtkImplicitPlaneRepresentation>::New();
	rep->SetPlaceFactor(1); // This must be set prior to placing the widget
	rep->PlaceWidget(data->GetBounds());
	rep->SetOrigin(data->GetCenter());
	rep->SetNormal(0, 0, 1);
	rep->SetOutlineTranslation(false);
	rep->SetScaleEnabled(false);

	auto cmd = vtkSmartPointer<NCommand>::New();
	cmd->Callback = [&](auto p)
	{
		vtkImplicitPlaneRepresentation* rep =
			reinterpret_cast<vtkImplicitPlaneRepresentation*>(m_PlaneWidget->GetRepresentation());
		auto plane = vtkSmartPointer<vtkPlane>::New();
		rep->GetPlane(plane);

		gMainWindow->ForEachRandomSlice([plane](auto slice) {
			slice->UpdateResliceByPlane(plane);
		});

		//auto slice = gMainWindow->GetRandomSlice();
		//if (slice)
		//{
		//	slice->UpdateResliceByPlane(plane);
		//}

	};

	m_PlaneWidget->AddObserver(vtkCommand::InteractionEvent, cmd);

	m_PlaneWidget->SetInteractor(m_RenderEngine->GetInteractor());
	m_PlaneWidget->SetRepresentation(rep);
	m_PlaneWidget->On();

	auto plane = vtkSmartPointer<vtkPlane>::New();
	rep->GetPlane(plane);

	gMainWindow->ForEachRandomSlice([plane](auto slice) {
		slice->UpdateResliceByPlane(plane);
	});
	

	if (m_VolumeBoundsBox.lock() == nullptr)
	{
		auto box = std::make_shared<BoxWidgetActor>();
	
		box->SetEnable(false);
		m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(box);

		m_VolumeBoundsBox = box;
	}

	m_VolumeBoundsBox.lock()->SetBounds(data->GetBounds());

}

void VolumeComponent::InitLocation3D(vtkImageData* data)
{
	if (m_LocationActor3D == nullptr)
	{
		auto p = std::make_shared<LocationActor3D>();
		p->SetVoxDim(data->GetDimensions());
		m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(p);

		m_LocationActor3D = p.get();
	}
	m_RenderEngine->GetRenderer()->ResetCamera(data->GetBounds());
	m_RenderEngine->Render();
}

void VolumeComponent::SetLocation3DPos(double* vpos)
{
	if (m_LocationActor3D)
	{
		m_LocationActor3D->SetVoxPos(vpos);
	}
}

void VolumeComponent::Render()
{
	if (m_RenderEngine)
	{
		m_RenderEngine->Render();
	}
}

void VolumeComponent::OnLabelTypeChanged(ColorLabelItem* labelType)
{
	auto model = gModelMgr->GetModel<MeshModel>();
	model->UpdateColor(labelType->ID.toInt(), labelType->Color);
	m_RenderEngine->Render();
}

void VolumeComponent::on_loadVolume_clicked()
{
	auto data = gLoadMgr->GetImageData();
	Init(data);
}

void VolumeComponent::on_updateMesh_clicked()
{
	//auto maskData = gLoadMgr->GetCurMaskData();
	//double range[2];
	//maskData->GetScalarRange(range);

	//int max = static_cast<int>( range[1] );
	//for (int i = 0; i < max; i++)
	//{
	//	auto mesh = std::make_shared<MeshActor>();
	//	mesh->LoadFromImageData(maskData, i);
	//	double rgb[3] = { 1, 0, 0 };
	//	mesh->SetColor(rgb);
	//	m_RenderEngine->GetSceneManager()->AddNodeToGlobleScene(mesh);
	//	m_Meshes.emplace_back(mesh);

	//}
	//if (!m_Meshes.empty())
	//{
	//	m_Meshes.back().lock()->ResetCameraToMe();
	//	m_RenderEngine->Render();
	//}

	auto model = gModelMgr->GetModel<MeshModel>();
	model->UpdateAll();
	model->ShowAll(ui->showMesh->isChecked());
	m_RenderEngine->Render();
}

void VolumeComponent::on_showMesh_clicked(bool v)
{
	auto model = gModelMgr->GetModel<MeshModel>();
	model->ShowAll(v);
	m_RenderEngine->Render();
}

void VolumeComponent::on_showVolume_clicked(bool v)
{
	if (m_VolumeActor)
	{
		m_VolumeActor->SetActive(v);
		m_RenderEngine->Render();
	}
}

void VolumeComponent::on_showPlaneWidget_clicked(bool v)
{
	if (m_PlaneWidget)
	{
		m_PlaneWidget->SetEnabled(v);
		m_RenderEngine->Render();
	}
}

void VolumeComponent::resizeEvent(QResizeEvent* event)
{
	if (event != nullptr)
	{
		auto size = ui->label->size();
		m_RenderEngine->SetRenWinSize(size.width(), size.height());
	}
}
