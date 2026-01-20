#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <string>
#include <vector>
#include <string_view>
#include <algorithm>
#include <ranges>
#include <charconv>

#include "simdutf.h"

namespace su
{

template <typename T>
constexpr bool is_char_ptr = std::is_same_v<const char*, std::decay_t<std::remove_cvref_t<T>>> || std::is_same_v<char*, std::decay_t<std::remove_cvref_t<T>>>;
template <typename T>
constexpr bool is_char16_ptr = std::is_same_v<const char16_t*, std::decay_t<std::remove_cvref_t<T>>> || std::is_same_v<char16_t*, std::decay_t<std::remove_cvref_t<T>>>;
template <typename T>
constexpr bool is_wchar_ptr = std::is_same_v<const wchar_t*, std::decay_t<std::remove_cvref_t<T>>> || std::is_same_v<wchar_t*, std::decay_t<std::remove_cvref_t<T>>>;



template <typename T> requires std::is_arithmetic_v<std::remove_cvref_t<T>>
inline size_t _impl_u8(char* out, size_t out_size, T val, int base)
{
	// Should be enough for anything
	char buffer[32];

	if (!out)
	{
		out = buffer;
		out_size = sizeof(buffer);
	}

	if constexpr (std::is_floating_point_v<std::remove_cvref_t<T>>)
	{
		if (auto r = std::to_chars(out, out + out_size, val); r.ec == std::errc())
			return r.ptr - out;
	}
	else
	{
		if (auto r = std::to_chars(out, out + out_size, val, base); r.ec == std::errc())
			return r.ptr - out;
	}

	assert(false);

	return 0;
}

template <typename T> requires (
	std::is_pointer_v<std::decay_t<std::remove_cvref_t<T>>>
	&& !is_char_ptr<T>
	&& !is_char16_ptr<T>
	&& !is_wchar_ptr<T>)
inline size_t _impl_u8(char* out, size_t out_size, T val, int base)
{
	return _impl_u8(out, out_size, (uintptr_t)val, 16);
}

inline size_t _impl_u8(char* out, size_t out_size, bool val, int base)
{
	if (!out)
		return val ? 4 : 5;

	if (val ? (out_size < 4) : (out_size < 5))
		return 0;

	if (val)
	{
		std::ranges::copy("true", out);
		return 4;
	}
	else
	{
		std::ranges::copy("false", out);
		return 5;
	}
}

inline size_t _impl_u8(char* out, size_t out_size, std::string_view view, int base)
{
	if (out)
	{
		if (out_size < view.size())
			return 0;

		for (size_t i = 0; i < view.size(); ++i)
			out[i] = view[i];
	}

	return view.size();
}

inline size_t _impl_u8(char* out, size_t out_size, const std::string& str, int base)
{
	return _impl_u8(out, out_size, std::string_view(str), base);
}

template <size_t N>
inline size_t _impl_u8(char* out, size_t out_size, const char (&str)[N], int base)
{
	return _impl_u8(out, out_size, std::string_view(str, N - 1), base);
}

inline size_t _impl_u8(char* out, size_t out_size, const char* str, int base)
{
	return _impl_u8(out, out_size, std::string_view(str), base);
}

inline size_t _impl_u8(char* out, size_t out_size, std::u16string_view view, int base)
{
	assert(simdutf::validate_utf16le(view.data(), view.size()));

	if (!out)
		return simdutf::utf8_length_from_utf16le(view.data(), view.size());

	assert(out_size >= simdutf::utf8_length_from_utf16le(view.data(), view.size()));
	return simdutf::convert_utf16le_to_utf8(view.data(), view.size(), out);
}

inline size_t _impl_u8(char* out, size_t out_size, const std::u16string& str, int base)
{
	return _impl_u8(out, out_size, std::u16string_view(str), base);
}

template <size_t N>
inline size_t _impl_u8(char* out, size_t out_size, const char16_t (&str)[N], int base)
{
	return _impl_u8(out, out_size, std::u16string_view(str, N - 1), base);
}

inline size_t _impl_u8(char* out, size_t out_size, const char16_t* str, int base)
{
	return _impl_u8(out, out_size, std::u16string_view(str), base);
}

inline size_t _impl_u8(char* out, size_t out_size, std::wstring_view str, int base)
{
	return _impl_u8(out, out_size, (const std::u16string_view&)str, base);
}

inline size_t _impl_u8(char* out, size_t out_size, const std::wstring& str, int base)
{
	return _impl_u8(out, out_size, (const std::u16string&)str, base);
}

template <size_t N>
inline size_t _impl_u8(char* out, size_t out_size, const wchar_t (&str)[N], int base)
{
	return _impl_u8(out, out_size, std::u16string_view((const char16_t*)str, N - 1), base);
}

inline size_t _impl_u8(char* out, size_t out_size, const wchar_t* str, int base)
{
	return _impl_u8(out, out_size, (const char16_t*)str, base);
}

template <typename T> requires std::is_arithmetic_v<std::remove_cvref_t<T>>
inline size_t _impl_u16(char16_t* out, size_t out_size, T val, int base = 10)
{
	char tmp_buffer[32];

	if (size_t size = _impl_u8<T>(tmp_buffer, sizeof(tmp_buffer), val, base))
	{
		if (out)
		{
			if (size > out_size)
			{
				assert(false);
				return 0;
			}

			for (size_t i = 0; i < size; ++i)
				out[i] = (char16_t)tmp_buffer[i];
		}

		return size;
	}

	assert(false);
	return 0;
}

template <typename T> requires (
	std::is_pointer_v<std::decay_t<std::remove_cvref_t<T>>>
	&& !is_char_ptr<T>
	&& !is_char16_ptr<T>
	&& !is_wchar_ptr<T>)
inline size_t _impl_u16(char16_t* out, size_t out_size, T val, int base)
{
	return _impl_u16(out, out_size, (uintptr_t)val, 16);
}

inline size_t _impl_u16(char16_t* out, size_t out_size, bool val, int base)
{
	if (!out)
		return val ? 4 : 5;

	if (val ? (out_size < 4) : (out_size < 5))
		return 0;

	if (val)
	{
		std::ranges::copy(u"true", out);
		return 4;
	}
	else
	{
		std::ranges::copy(u"false", out);
		return 5;
	}
}

inline size_t _impl_u16(char16_t* out, size_t out_size, const std::string_view& view, int base)
{
	assert(simdutf::validate_utf8(view.data(), view.size()));

	if (!out)
		return simdutf::utf16_length_from_utf8(view.data(), view.size());

	assert(out_size >= simdutf::utf16_length_from_utf8(view.data(), view.size()));
	return simdutf::convert_utf8_to_utf16le(view.data(), view.size(), out);
}

inline size_t _impl_u16(char16_t* out, size_t out_size, const std::string& str, int base)
{
	return _impl_u16(out, out_size, str, base);
}

inline size_t _impl_u16(char16_t* out, size_t out_size, const char* str, int base)
{
	return _impl_u16(out, out_size, std::string_view(str), base);
}

template <typename T>
inline size_t u8(char* out, size_t out_size, const T& val, int base = 10)
{
	return _impl_u8(out, out_size, val, base);
}

template <typename T>
inline std::string u8(const T& val, int base = 10)
{
	size_t size = u8<T>(nullptr, 0, val);
	std::string result(size, '\0');
	u8<T>(result.data(), size, val, base);
	return result;
}

template <typename T>
inline size_t u16(char16_t* out, size_t out_size, const T& val, int base = 10)
{
	return _impl_u16(out, out_size, val, base);
}

template <typename T>
inline std::u16string u16(const T& val, int base = 10)
{
	size_t size = u16<T>(nullptr, 0, val);
	std::u16string result(size, '\0');
	u16<T>(result.data(), size, val, base);
	return result;
}


template <typename TChar>
inline auto SplitView(const std::basic_string_view<TChar>& input, const std::basic_string_view<TChar>& delimiter)
{
	auto to_string = [](auto&& range) {
		if (std::ranges::empty(range))
			return std::basic_string_view<TChar>{};

		return std::basic_string_view<TChar>(
			&*std::ranges::begin(range),
			std::ranges::size(range)
		);
	};

	return input |
		std::views::split(delimiter) |
		std::views::transform(to_string);
}

template <typename TChar>
inline std::vector<std::basic_string<TChar>> Split(const std::basic_string_view<TChar>& input, const std::basic_string_view<TChar>& delimiter)
{
	auto view = SplitView(input, delimiter);
	return std::vector(view.begin(), view.end());
}

} // namespace su
