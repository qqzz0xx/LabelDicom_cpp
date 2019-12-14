#pragma once
#include "json.hpp"
#include "glm.hpp"
#include <QColor>

namespace JsonHelper
{
	template<class T>
	inline void TryGetValue(const nlohmann::json& j, T& outVal)
	{
		if (!j.is_null())
		{
			outVal = j.get<T>();
		}
	}

	template<class T>
	inline void TryGetValue(const nlohmann::json& j, const std::string& key, T& outVal)
	{
		if (!j.is_null())
		{
			if (auto it = j.find(key); it != j.end())
			{
				outVal = it.value();
			}
		}
	}

	inline void ArrayToJson(const double* array, int size, nlohmann::json& outJson)
	{
		for (int i = 0; i < size; ++i)
		{
			outJson.push_back(array[i]);
		}
	}

	inline void JsonToArray(const nlohmann::json& j, double* array, int size)
	{
		if (!j.is_null() && j.is_array())
		{
			for (int i = 0; i < j.size() && i < size; ++i)
			{
				*(array + i) = j[i];
			}
		}
	}

	inline void JsonToQColor(const nlohmann::json& j, QColor& color)
	{
		if (!j.is_null() && j.is_array())
		{
			color.setRgb(j[0], j[1], j[2], j[3]);
		}
	}

	inline void QColorToJson(const QColor& color, nlohmann::json& j)
	{
		j.push_back(color.red());
		j.push_back(color.green());
		j.push_back(color.blue());
		j.push_back(color.alpha());
	}

	inline void Vec2ToJson(const glm::dvec2& v, nlohmann::json& j)
	{
		j.push_back(v.x);
		j.push_back(v.y);
	}

	inline void Vec3ToJson(const glm::dvec3& v, nlohmann::json& j)
	{
		j.push_back(v.x);
		j.push_back(v.y);
		j.push_back(v.z);
	}

	inline void JsonToVec2(const nlohmann::json& j, glm::dvec2& v)
	{
		v.x = j[0].get<double>();
		v.y = j[1].get<double>();
	}

	inline void JsonToVec3(const nlohmann::json& j, glm::dvec3& v)
	{
		v.x = j[0].get<double>();
		v.y = j[1].get<double>();
		v.z = j[2].get<double>();
	}
}