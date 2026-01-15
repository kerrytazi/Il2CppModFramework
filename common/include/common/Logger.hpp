#pragma once

#include "common/StyledString.hpp"

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

	// Called each time you call any function from the Log namespace.
	// It's up to you to filter messages by their level and type.
	virtual void AddLine(Level level, Type type, std::initializer_list<cs::StyledString> line);

	size_t GetSystemTimeStr(char* out, size_t out_size);
	std::string GetSystemTimeStr();
};
