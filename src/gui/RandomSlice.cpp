#include "RandomSlice.h"
#include "NCommand.h"
#include "MathHelper.h"
#include "CameraZoomAndMoveTools.h"
//#include "BoundingBox3DTools.h"
#include "RandomSliceBoxTools.h"
#include "SliceCutterActor.h"
#include "MainWindow.h"
#include "AppMain.h"

using namespace NRender;

RandomSlice::RandomSlice(QWidget* parent /*= Q_NULLPTR*/):
	ui(new Ui_RandomSlice)
{
	ui->setupUi(this);
	m_View = ui->view;

	auto tools = m_View->GetTools();
	auto func = [&](auto t)
	{
		t->Init(ui->view);
		tools->Add(t);
	};

	func(std::make_shared<RandomSliceBoxTools>());
	func(std::make_shared<CameraZoomAndMoveTools>());
}

RandomSlice::~RandomSlice()
{
}

void RandomSlice::Init(vtkImageData* data, MprType type)
{
	m_Type = type;

	switch (m_Type)
	{
	case RandomX:
		m_Reslice.Init(data, MprType::Sagittal);
		break;
	case RandomY:
		m_Reslice.Init(data, MprType::Coronal);
		break;
	case RandomZ:
		m_Reslice.Init(data, MprType::Axial);
		break;
	default:
		break;
	}

	m_View->Init(m_Reslice.GetImageData(), m_Type);
	m_View->GetLocationActor()->SetActive(false);

	auto vc = gMainWindow->GetMprComponent()->GetVolumeComponent();
	if (vc)
	{
		auto pw = vc->GetPlaneWidget();
		vtkImplicitPlaneRepresentation* rep =
			reinterpret_cast<vtkImplicitPlaneRepresentation*>(pw->GetRepresentation());
		auto plane = vtkSmartPointer<vtkPlane>::New();
		rep->GetPlane(plane);
		UpdateResliceByPlane(plane);
	}

	m_View->Render();
}

void RandomSlice::UpdateResliceByPlane(vtkPlane* plane)
{
	auto origin = glm::make_vec3(plane->GetOrigin());
	auto normal = glm::normalize(glm::make_vec3(plane->GetNormal()));
	auto x = glm::dvec3(1, 0, 0);
	auto y = glm::normalize(glm::cross(x, normal));
	auto b = glm::isnan(y);
	if (b.x || b.y || b.z)
	{
		y = glm::dvec3(0, 1, 0);
	}

	x = glm::normalize(glm::cross(y, normal));
	CompResliceMatrix(x, y, normal, origin, m_Reslice.GetResliceMatrix());
	//MakeMatrix(x, y, normal, origin, m_Reslice.GetResliceMatrix());
	
	m_Reslice.Update();
	m_View->GetSingleImageActor()->Refresh();

	auto scene = m_View->GetRenderEngine()->GetSceneManager()->GetGlobleScene();
	auto&& nodes = scene->NodeList();
	for (auto&& var : nodes)
	{
		if (var->GetActive() && var->GetName() == "SliceCutterActor")
		{
			auto p = (SliceCutterActor*)var.get();
			p->Refresh();
		}
	}

	m_View->Render();
}

void RandomSlice::CompResliceMatrix(const glm::dvec3& x, const glm::dvec3& y, 
	 const glm::dvec3& z, const glm::dvec3& o, vtkMatrix4x4* mat)
{
	switch (m_Type)
	{
	case RandomX:
		MakeMatrix(-z, y, x, o, mat);
		break;
	case RandomY:
		MakeMatrix(x, -z, y, o, mat);
		break;
	case RandomZ:
		MakeMatrix(x, y, z, o, mat);
		break;
	default:
		break;
	}
	
}

void RandomSlice::showEvent(QShowEvent* event)
{
	auto tools = m_View->GetTools();
	tools->DoIf<RandomSliceBoxTools>([](auto t)
	{
		t->SetEnable(true);
	});
}

void RandomSlice::hideEvent(QHideEvent* event)
{
	auto tools = m_View->GetTools();
	tools->DoIf<RandomSliceBoxTools>([](auto t)
	{
		t->SetEnable(false);
	});
}

