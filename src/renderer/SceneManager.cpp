#include "SceneManager.h"
#include "RenderEngine.h"

using namespace NRender;

SceneManager::SceneManager(RenderEngine* renderEngine)
{
	m_RenderEngine = renderEngine;
}

SceneManager::~SceneManager()
{
}

void NRender::SceneManager::OnMouseEvent(MouseEventType type)
{

	if (m_GlobleScene) m_GlobleScene->OnMouseEvent(type);
	for (auto & var : m_SceneStack)
	{
		var->OnMouseEvent(type);
	}
}

void NRender::SceneManager::Start()
{
	m_GlobleScene = std::make_shared<Scene>();
	m_GlobleScene->SetRenderer(m_RenderEngine);
	m_GlobleScene->Start();
}

void NRender::SceneManager::Update(float dt)
{
	if (m_GlobleScene) m_GlobleScene->Update(dt);
	for (auto& var : m_SceneStack)
	{
		var->Update(dt);
	}
}

void NRender::SceneManager::Exit()
{
	if (m_GlobleScene) m_GlobleScene->Exit();
}

std::shared_ptr<SceneNode> NRender::SceneManager::FindNode(const std::string& name)
{
	if (m_GlobleScene)
	{
		auto node = m_GlobleScene->FindNode(name);
		if (node != nullptr) return node;
	}
	for (auto& var : m_SceneStack)
	{
		auto node = var->FindNode(name);
		if (node != nullptr) return node;
	}

	return nullptr;
}

std::shared_ptr<SceneNode> NRender::SceneManager::FindNodeByID(const std::string & id)
{
	if (m_GlobleScene)
	{
		auto node = m_GlobleScene->FindNodeByID(id);
		if (node != nullptr) return node;
	}
	for (auto& var : m_SceneStack)
	{
		auto node = var->FindNodeByID(id);
		if (node != nullptr) return node;
	}

	return nullptr;
}

std::shared_ptr<Scene> NRender::SceneManager::GetCurScene()
{
	if (!m_SceneStack.empty())
	{
		return m_SceneStack.back();
	}
	return nullptr;
}

void NRender::SceneManager::SwitchScene(std::shared_ptr<Scene> scene)
{
	auto curScene = GetCurScene();
	if (curScene && (curScene->GetName() == scene->GetName())) return;

	PopScene();

	PushScene(scene);
}

void NRender::SceneManager::PushScene(std::shared_ptr<Scene> scene)
{
	if (scene == nullptr) return;
	scene->SetRenderer(m_RenderEngine);
	scene->Start();
	m_SceneStack.emplace_back(scene);
}

void NRender::SceneManager::PopScene()
{
	if (!m_SceneStack.empty())
	{
		m_SceneStack.back()->Exit();
		m_SceneStack.pop_back();
	}
}

void NRender::SceneManager::AddNodeToCurScene(std::shared_ptr<SceneNode> node)
{
	auto curScene = GetCurScene();
	if(curScene != nullptr)
	{
		curScene->AddNode(node);
	}
}

void NRender::SceneManager::AddNodeToGlobleScene(std::shared_ptr<SceneNode> node)
{
	if (m_GlobleScene)
	{
		m_GlobleScene->AddNode(node);
	}
}

void NRender::SceneManager::AddNodeToScene(std::shared_ptr<SceneNode> node, const std::string & sceneName)
{
	if (m_GlobleScene && m_GlobleScene->GetName() == sceneName)
	{
		m_GlobleScene->AddNode(node);
	}
	else
	{
		for (auto& var : m_SceneStack)
		{
			if (var && var->GetName() == sceneName)
			{
				var->AddNode(node);
			}
		}
	}
}

void NRender::SceneManager::RemoveNode(std::shared_ptr<SceneNode> node)
{
	if (node == nullptr) return;
	m_GlobleScene->RemoveNode(node);
	for (auto &var : m_SceneStack)
	{
		var->RemoveNode(node);
	}
}

void NRender::SceneManager::ClearScene()
{
	if (m_GlobleScene != nullptr)
	{
		m_GlobleScene->Exit();
	}
	m_GlobleScene = std::make_shared<Scene>();
	m_GlobleScene->Start();

	for (auto& var : m_SceneStack)
	{
		var->Exit();
	}
	m_SceneStack.clear();
}

void NRender::SceneManager::RemoveNodeByName(const std::string & name)
{
	auto node = FindNode(name);
	RemoveNode(node);
}

void NRender::SceneManager::RemoveNodeByID(const std::string & id)
{
	auto node = FindNodeByID(id);
	RemoveNode(node);
}

std::shared_ptr<Scene> NRender::SceneManager::FindScene(const std::string & name)
{
	if (m_GlobleScene && m_GlobleScene->GetName() == name)
	{
		return m_GlobleScene;
	}
	for (auto& var : m_SceneStack)
	{
		if (var && var->GetName() == name)
		{
			return var;
		}
	}
	return nullptr;
}

std::vector<std::shared_ptr<Scene>> NRender::SceneManager::SceneList()
{
	std::vector<std::shared_ptr<Scene>> scenes;
	scenes.emplace_back(m_GlobleScene);
	scenes.insert(scenes.end(),  m_SceneStack.begin(), m_SceneStack.end());
	return scenes;
}
