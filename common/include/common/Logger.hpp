#pragma once

#include "common/ColoredString.hpp"

#include <string>
#include <string_view>
#include <initializer_list>
#include <compare>

class Logger
{
public:

	enum class Level
	{
		Debug,
		Info,
		Warning,
		Error,

		Default = Info,
	};

	friend auto operator<=>(Level lhs, Level rhs)
	{
		return int(lhs) <=> int(rhs);
	}

	enum class Type
	{
		System,
		InGame,

		Default = System,
	};

	virtual ~Logger();
	virtual void AddLine(Level level, Type type, std::initializer_list<cs::ColoredString> line);

	std::string GetSystemTimeStr();
};
