#pragma once

#include <string_view>

namespace cs
{

enum class Color
{
	Normal,
};

struct ColoredString
{
	std::string_view str;
	Color color = Color::Normal;
};

static constexpr ColoredString normal(std::string_view str)
{
	return ColoredString{
		.str = str,
		.color = Color::Normal,
	};
}

} // namespace cs
