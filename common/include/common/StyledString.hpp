#pragma once

#include <string_view>

namespace cs
{

// If colors ever change this version will be incremented.
// You can add:
// static_assert(_COLORS_VERSION == 1, "StyledString version doesn't match");
// to your code if you depend on colors order and count.
constexpr int _COLORS_VERSION = 1;

enum class Color
{
	Default,
	White,
	Black,
	Gray,
	Red,
	Green,
	Blue,
	Yellow,
	Orange,
	Pink,
	Cyan,
	Purple,
	Magenta,
};

template <typename T>
struct Styled
{
	T value;
	Color color = Color::Default;
};

using StyledString = Styled<std::string_view>;

template <typename T> constexpr Styled<T> Default(T _value) { return Styled<T>{ _value, Color::Default }; };
template <typename T> constexpr Styled<T> White(T _value) { return Styled<T>{ _value, Color::White }; };
template <typename T> constexpr Styled<T> Black(T _value) { return Styled<T>{ _value, Color::Black }; };
template <typename T> constexpr Styled<T> Gray(T _value) { return Styled<T>{ _value, Color::Gray }; };
template <typename T> constexpr Styled<T> Red(T _value) { return Styled<T>{ _value, Color::Red }; };
template <typename T> constexpr Styled<T> Green(T _value) { return Styled<T>{ _value, Color::Green }; };
template <typename T> constexpr Styled<T> Blue(T _value) { return Styled<T>{ _value, Color::Blue }; };
template <typename T> constexpr Styled<T> Yellow(T _value) { return Styled<T>{ _value, Color::Yellow }; };
template <typename T> constexpr Styled<T> Orange(T _value) { return Styled<T>{ _value, Color::Orange }; };
template <typename T> constexpr Styled<T> Pink(T _value) { return Styled<T>{ _value, Color::Pink }; };
template <typename T> constexpr Styled<T> Cyan(T _value) { return Styled<T>{ _value, Color::Cyan }; };
template <typename T> constexpr Styled<T> Purple(T _value) { return Styled<T>{ _value, Color::Purple }; };

template <typename T> constexpr const T& UnrapStyled(const T& val) { return val; }
template <typename T> constexpr const T& UnrapStyled(const cs::Styled<T>& colored) { return colored.value; }

template <typename T> constexpr cs::Color UnrapColor(const T& val) { return cs::Color::Default; }
template <typename T> constexpr cs::Color UnrapColor(const cs::Styled<T>& colored) { return colored.color; }

} // namespace cs
