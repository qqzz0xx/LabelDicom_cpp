#pragma once

#include <map>
#include <memory>
template<class T>
class Table
{
public:
	template<class U> void Add(std::shared_ptr<U> ptr);

	template<class U> U* Get();
	T* Get(const std::string& name);

	void ForEach(const std::function<void(T*)>& call) {
		for (auto& var : m_Map)
		{
			call(var.second.get());
		}
	}

	void ForEach(const std::function<void(const std::string&, T*)>& call) {
		for (auto&& [k, v] : m_Map)
		{
			call(k, v.get());
		}
	}

	template<class ...Args> void ForEachNoContain(std::function<void(T*)> call);

	template<class U> void DoIf(std::function<void(U*)> call);
	void DoIf(const std::string& name, std::function<void(T*)> call);

private:
	std::map<std::string, std::shared_ptr<T>> m_Map;
};



template<class T>
template<class U>
inline void Table<T>::Add(std::shared_ptr<U> ptr)
{
	auto name = typeid(U*).name();
	m_Map.emplace(name, ptr);
}

template<class T>
template<class U>
inline U* Table<T>::Get()
{
	auto name = typeid(U*).name();
	auto it = m_Map.find(name);
	if (it != m_Map.end())
	{
		return (U*)it->second.get();
	}
	return nullptr;
}

template<class T>
inline T * Table<T>::Get(const std::string & name)
{
	auto it = m_Map.find(name);
	if (it != m_Map.end())
	{
		return it->second.get();
	}
	return nullptr;
}

template<class T>
template<class ...Args>
inline void Table<T>::ForEachNoContain(std::function<void(T*)> call)
{
	std::initializer_list<std::string> arr = { typeid(Args*).name()... };
	bool isEq = false;
	for (auto& var : m_Map)
	{
		for (auto& n : arr)
		{
			if (n == var.first)
			{
				isEq = true;
				break;
			}
		}
		if (!isEq)
		{
			call(var.second.get());
		}
	}
}

template<class T>
template<class U>
inline void Table<T>::DoIf(std::function<void(U*)> call)
{
	auto p = Get<U>();
	if (p != nullptr)
	{
		call(p);
	}
}

template<class T>
inline void Table<T>::DoIf(const std::string & name, std::function<void(T*)> call)
{
	auto p = Get(name);
	if (p != nullptr)
	{
		call(p);
	}
}
