#pragma once
#include "Header.h"
#include "Behavior.h"
#include "Scene.h"
#include "Types.h"

namespace NRender
{
	class RenderEngine;

	class SceneManager : public Behavior
	{
	public:
		virtual void Start() override;
		virtual void Update(float dt) override;
		virtual void Exit() override;
		void OnMouseEvent(MouseEventType type) ;

		std::shared_ptr<SceneNode> FindNode(const std::string& name);
		std::shared_ptr<SceneNode> FindNodeByID(const std::string& id);

		std::shared_ptr<Scene> GetCurScene();
		void SwitchScene(std::shared_ptr<Scene> scene);
		void PushScene(std::shared_ptr<Scene> scene);
		void PopScene();
		void AddNodeToCurScene(std::shared_ptr<SceneNode> node);
		void AddNodeToGlobleScene(std::shared_ptr<SceneNode> node);
		void AddNodeToScene(std::shared_ptr<SceneNode> node, const std::string& sceneName);

		void RemoveNodeByName(const std::string& name);
		void RemoveNodeByID(const std::string& id);
		void RemoveNode(std::shared_ptr<SceneNode> node);
		void ClearScene();

		std::shared_ptr<Scene> FindScene(const std::string& name);
		std::vector<std::shared_ptr<Scene>> SceneList();
		std::shared_ptr<Scene> GetGlobleScene() { return m_GlobleScene; }
	public:
		SceneManager(RenderEngine* renderer);
		~SceneManager();
	private:
		std::shared_ptr<Scene> m_GlobleScene;
		std::vector<std::shared_ptr<Scene>> m_SceneStack;

		RenderEngine* m_RenderEngine = nullptr;
	};
}

