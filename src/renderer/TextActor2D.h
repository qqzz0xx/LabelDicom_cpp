#pragma once

#include "Header.h"
#include "Behavior.h"
#include "SceneNode.h"
#include "glm.hpp"
#include "vtkTextActor.h"

namespace NRender {
	class TextActor2D : public SceneNode
	{
	public:
		TextActor2D();

		void SetText(const std::string& txt);
		std::string GetText();

		void SetPosition(const glm::dvec2& pos);
		
		void SetTextColor(double* color);

		virtual void Update(float dt) override;

		virtual void Start() override;

		virtual void Exit() override;

		vtkSmartPointer<vtkTextActor> m_Actor;

	private:
	};
}