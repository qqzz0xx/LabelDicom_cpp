#pragma once
#include "Header.h"
namespace Dispatcher
{
	enum Event
	{

	};

	const std::function<void(Event)>& On(const Event& ev, const std::function<void(Event)>& callback);
	void Off(const std::function<void(Event)>& callback);
	void Fire(Event ev);
}

