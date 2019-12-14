#include "Dispatcher.h"
#include "uuid/uuid4.h"

using namespace Dispatcher;

static std::unordered_map<std::string, std::function<void(Event)>> s_EventList;
static std::unordered_map<Event, std::vector<std::string>> s_ListenerList;

const std::function<void(Event)>& On(const Event& ev, const std::function<void(Event)>& callback)
{
	auto uuid = GenUUID();
	s_EventList.emplace(uuid, callback);

	auto it = s_ListenerList.find(ev);
	if (it == s_ListenerList.end())
	{
		s_ListenerList[ev] = std::vector<std::string>();
	}
	s_ListenerList[ev].emplace_back(uuid);

	return callback;
}

void Off(const std::string& uuid)
{
	for (auto& [k, v] : s_ListenerList)
	{
		for (auto it = v.begin(); it != v.end(); it++)
		{
			
			
		}
	}
}

void Fire(Event ev)
{
	auto it = s_ListenerList.find(ev);
	if (it != s_ListenerList.end())
	{
		for (auto&& var : s_ListenerList[ev])
		{
			auto it = s_EventList.find(var);

		}
	}
}
