#pragma once
namespace NRender
{
	class Behavior
	{
	public:
		virtual void Start() = 0;
		virtual void Update(float dt) = 0;
		virtual void Exit() = 0;
	};
}

