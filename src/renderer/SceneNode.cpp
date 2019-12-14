#include "SceneNode.h"
#include "MathHelper.h"
#include "RenderEngine.h"

using namespace NRender;

SceneNode::SceneNode()
{
	m_IsActive = true;
	m_IsActiveSelf = true;
	m_IsEnable = true;
	m_ParentNode = nullptr;
	m_Actor = nullptr;

	m_WorldMat = vtkSmartPointer<vtkMatrix4x4>::New();
	m_LocalMat = vtkSmartPointer<vtkMatrix4x4>::New();
}

SceneNode::~SceneNode()
{
	Exit();
	for (auto& child : m_ChildNodes)
	{
		child->Exit();
	}
	if (m_Actor != NULL)
	{
		m_Actor->Delete();
	}
}

void NRender::SceneNode::SetActive(bool isShow)
{
	m_IsActiveSelf = isShow;
	for (auto& var : m_ChildNodes)
	{
		var->SetActiveParent(m_IsActiveSelf && m_IsActive);
	}

	SetActiveSelf(m_IsActiveSelf && m_IsActive);

}

void NRender::SceneNode::SetActiveParent(bool isShow)
{
	m_IsActive = isShow;

	for (auto& var : m_ChildNodes)
	{
		var->SetActiveParent(m_IsActiveSelf && m_IsActive);
	}

	SetActiveSelf(m_IsActiveSelf && m_IsActive);
}

void NRender::SceneNode::SetParent(SceneNode * parent)
{
	if (parent == nullptr) return;
	m_ParentNode = parent;
	m_RenderEngine = parent->m_RenderEngine;
	//VTK_PTR(vtkMatrix4x4, parentMatInv);
	//parentMatInv->DeepCopy(parent->GetWorldMat());
	//parentMatInv->Invert();

	CompDeltaMat(m_WorldMat, parent->GetWorldMat(), m_LocalMat);
}

void NRender::SceneNode::CheckPickAndMove(vtkProp3D * prop)
{
	//if (MainAppInst()->GetInputManager()->IsPicked(prop))
	//{
	//	auto oldWorldMat = vtkSmartPointer<vtkMatrix4x4>::New();
	//	oldWorldMat->DeepCopy(m_WorldMat);

	//	auto deltaMat = MainAppInst()->GetInputManager()->GetMouseDeltaMat(prop);

	//	vtkMatrix4x4::Multiply4x4(deltaMat, oldWorldMat, oldWorldMat);

	//	SetWorldMat(oldWorldMat);
	//}
}

void NRender::SceneNode::UpdateMatrix()
{
	//VTK_PTR(vtkMatrix4x4, mat);

	//VTK_PTR(vtkTransform, transform);

}

void NRender::SceneNode::OnMouseEvent(MouseEventType type)
{
}

void NRender::SceneNode::ResetCameraToMe()
{
	if (m_RenderEngine && m_Actor)
	{
		m_RenderEngine->GetRenderer()->ResetCamera(m_Actor->GetBounds());
	}
}

vtkPolyData * NRender::SceneNode::GetPolydata()
{
	auto actor = vtkActor::SafeDownCast(m_Actor);
	if (actor != nullptr)
	{
		auto mapper = vtkPolyDataMapper::SafeDownCast(actor->GetMapper());
		if (mapper)
		{
			return mapper->GetInput();
		}
	}

	return nullptr;
}

void NRender::SceneNode::SetActiveSelf(bool isShow)
{
	if (m_Actor)
	{
		m_Actor->SetVisibility(isShow);
	}

}

bool NRender::SceneNode::GetActive()
{
	return m_Actor && m_Actor->GetVisibility();
}

bool NRender::SceneNode::GetActiveSelf()
{
	return m_IsActiveSelf;
}

void NRender::SceneNode::SetEnable(bool isEnable)
{
	m_IsEnable = isEnable;
}

bool NRender::SceneNode::GetEnable()
{
	return m_IsEnable;
}

void NRender::SceneNode::SetPickable(bool v)
{
	if (m_Actor)
	{
		m_Actor->SetPickable(v);
	}
}

void NRender::SceneNode::AddChild(std::shared_ptr<SceneNode> node)
{
	if (node == nullptr) return;
	m_ChildNodes.emplace_back(node);
	node->SetActiveParent(m_IsActive);
	node->SetParent(this);
	node->SetRenderer(m_RenderEngine);
	node->Start();
}

void NRender::SceneNode::RemoveChild(std::shared_ptr<SceneNode> node)
{
	for (auto it = m_ChildNodes.begin(); it != m_ChildNodes.end(); ++it)
	{
		if (*it == node)
		{
			node->SetParent(nullptr);
			node->Exit();
			m_ChildNodes.erase(it);
			break;
		}
	}
}

std::shared_ptr<SceneNode> NRender::SceneNode::FindChild(const std::string & name)
{
	for (auto it = m_ChildNodes.begin(); it != m_ChildNodes.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			return *it;
		}
	}
	return nullptr;
}

std::shared_ptr<SceneNode> NRender::SceneNode::FindChildByID(const std::string & id)
{
	for (auto it = m_ChildNodes.begin(); it != m_ChildNodes.end(); ++it)
	{
		if ((*it)->GetUUID() == id)
		{
			return *it;
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<SceneNode>> NRender::SceneNode::GetChildList()
{
	return m_ChildNodes;
}

vtkMatrix4x4 * NRender::SceneNode::GetWorldMat()
{
	return m_WorldMat;
}

vtkMatrix4x4 * NRender::SceneNode::GetLocalMat()
{
	if (m_ParentNode == nullptr) return m_WorldMat;
	CompDeltaMat(m_WorldMat, m_ParentNode->GetWorldMat(), m_LocalMat);
	return m_LocalMat;
}

void NRender::SceneNode::SetWorldMat(vtkMatrix4x4 * mat)
{
	auto deltaMat = vtkSmartPointer<vtkMatrix4x4>::New();
	CompDeltaMat(mat, m_WorldMat, deltaMat);
	m_WorldMat->DeepCopy(mat);
	m_WorldMat->Modified();
	for (auto& var : m_ChildNodes)
	{
		auto mat = var->GetWorldMat();
		vtkMatrix4x4::Multiply4x4(deltaMat, mat, mat);
		mat->Modified();
	}

}

void NRender::SceneNode::SetLocalMat(vtkMatrix4x4 * mat)
{
	if (m_ParentNode == nullptr)
	{
		SetWorldMat(mat);
	}
	else
	{
		auto worldMat = vtkSmartPointer<vtkMatrix4x4>::New();
		vtkMatrix4x4::Multiply4x4(m_ParentNode->GetWorldMat(), mat, worldMat);

		SetWorldMat(worldMat);
	}
}

void NRender::SceneNode::SetRotation(const glm::dvec3 & rotate)
{
	glm::dquat myquaternion = glm::quat(rotate);
	auto mat = glm::mat4_cast(myquaternion);

	mat = glm::inverse(mat);

	vtkMatrix4x4::Multiply4x4(m_WorldMat->Element[0], glm::value_ptr(mat), m_WorldMat->Element[0]);
	m_WorldMat->Modified();
}

void NRender::SceneNode::SetRotation(const double * rot)
{
	glm::dvec3 rotRadians(glm::radians(rot[0]), glm::radians(rot[1]), glm::radians(rot[2]));
	SetRotation(rotRadians);
}

void NRender::SceneNode::SetPosition(double *p)
{
	if (m_Actor != NULL)
	{
		m_Actor->SetPosition(p);
	}
}

void NRender::SceneNode::SetOrigin(const glm::dvec3 & origin)
{
}

void NRender::SceneNode::SetColor(double * rgb)
{
	auto actor = vtkActor::SafeDownCast(m_Actor);
	if (actor) actor->GetProperty()->SetColor(rgb);
}

void NRender::SceneNode::SetAlpha(double alpha)
{
	auto actor = vtkActor::SafeDownCast(m_Actor);
	if (actor) actor->GetProperty()->SetOpacity(alpha);
}

std::string NRender::SceneNode::GetName()
{
	return m_Name;
}

void NRender::SceneNode::SetName(const std::string & name)
{
	m_Name = name;
}

std::string NRender::SceneNode::GetUUID()
{
	return m_UUID;
}

void NRender::SceneNode::SetUUID(const std::string & id)
{
	m_UUID = id;
}

void NRender::SceneNode::SetRenderer(RenderEngine* renderer)
{
	m_RenderEngine = renderer;
	for (auto&& var : m_ChildNodes)
	{
		var->SetRenderer(renderer);
	}
}

void NRender::SceneNode::Update(float dt)
{
	if (!m_IsEnable) return;

	for (auto& var : m_ChildNodes)
	{
		var->Update(dt);
	}

	CheckPickAndMove(m_Actor);
}

void NRender::SceneNode::Start()
{
	if (m_Actor && m_RenderEngine)
	{
		m_RenderEngine->GetRenderer()->AddViewProp(m_Actor);
	}

	for (auto& var : m_ChildNodes)
	{
		var->Start();
	}
}

void NRender::SceneNode::Exit()
{
	if (m_Actor && m_RenderEngine)
	{
		m_RenderEngine->GetRenderer()->RemoveViewProp(m_Actor);
	}

	for (auto& var : m_ChildNodes)
	{
		var->Exit();
	}
}

