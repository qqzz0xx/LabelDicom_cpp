#pragma once
#include "Header.h"
#include "Behavior.h"
#include "SceneNode.h"
#include "Types.h"

namespace NRender
{
	class RenderEngine;
	class Scene : public Behavior
	{
	public:
		Scene();
		~Scene();

		void SetRenderer(RenderEngine* renderer);
		virtual void Start() override;
		virtual void Update(float dt) override;
		virtual void Exit() override;
		void OnMouseEvent(MouseEventType type);

	public:
		std::string GetName();
		void AddNode(std::shared_ptr<SceneNode> node);
		void RemoveNode(std::shared_ptr<SceneNode> node);
		std::shared_ptr<SceneNode> FindNode(const std::string& name);
		std::shared_ptr<SceneNode> FindNodeByID(const std::string& id);
		std::vector<std::shared_ptr<SceneNode>> NodeList() { return m_Nodes; }
	protected:
		std::string m_Name;
		std::vector<std::shared_ptr<SceneNode>> m_Nodes;

		RenderEngine* m_Renderer = nullptr;

	
	};
}

