#include "Scene.h"

using namespace NRender;

Scene::Scene()
{
}

Scene::~Scene()
{
}

void NRender::Scene::SetRenderer(RenderEngine* renderer)
{
	m_Renderer = renderer;
}

void NRender::Scene::Start()
{
	for (auto& var : m_Nodes)
	{
		var->Start();
	}
}

void NRender::Scene::Update(float dt)
{
	for (auto& var : m_Nodes)
	{
		if(var->GetActive())
		{
			var->Update(dt);
		}
	}
}

void NRender::Scene::Exit()
{
	for (auto& var : m_Nodes)
	{
		var->Exit();
	}
}

std::string NRender::Scene::GetName()
{
	return m_Name;
}

void NRender::Scene::AddNode(std::shared_ptr<SceneNode> node)
{
	if (node == nullptr) return;
	m_Nodes.emplace_back(node);
	node->SetRenderer(m_Renderer);
	node->Start();
}

void NRender::Scene::RemoveNode(std::shared_ptr<SceneNode> node)
{
	for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
	{
		if (*it == node)
		{
			node->Exit();
			m_Nodes.erase(it);
			break;
		}
		else
		{
			(*it)->RemoveChild(node);
		}
	}
}

std::shared_ptr<SceneNode> NRender::Scene::FindNode(const std::string& name)
{
	for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
	{
		if ((*it)->GetName() == name)
		{
			return *it;
		}

		auto child = (*it)->FindChild(name);
		if (child != nullptr) return child;
	}
	return nullptr;
}

std::shared_ptr<SceneNode> NRender::Scene::FindNodeByID(const std::string& id)
{
	for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
	{
		if ((*it)->GetUUID() == id)
		{
			return *it;
		}

		auto child = (*it)->FindChildByID(id);
		if (child != nullptr) return child;
	}
	return nullptr;
}

void NRender::Scene::OnMouseEvent(MouseEventType type)
{
	for (auto & var : m_Nodes)
	{
		if (var->GetEnable())
		{
			var->OnMouseEvent(type);
		}
	}
}

