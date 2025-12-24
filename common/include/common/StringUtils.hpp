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
size_t u8(char* out, size_t out_size, const T& val)
{
	// Should be enough for anything
	char buffer[32];

	if (!out)
	{
		out = buffer;
		out_size = sizeof(buffer);
	}

	if (auto r = std::to_chars(out, out + out_size, val); r.ec != std::errc())
		return r.ptr - out;

	assert(false);

	return 0;
}

template <>
inline size_t u8<std::u16string_view>(char* out, size_t out_size, const std::u16string_view& view)
{
	assert(simdutf::validate_utf16le(view.data(), view.size()));

	if (!out)
		return simdutf::utf8_length_from_utf16le(view.data(), view.size());

	assert(out_size >= simdutf::utf8_length_from_utf16le(view.data(), view.size()));
	return simdutf::convert_utf16le_to_utf8(view.data(), view.size(), out);
}

template <>
inline size_t u8<std::u16string>(char* out, size_t out_size, const std::u16string& str)
{
	return u8<std::u16string_view>(out, out_size, str);
}

template <>
inline size_t u8<const char16_t*>(char* out, size_t out_size, char16_t const* const& str)
{
	return u8<std::u16string_view>(out, out_size, str);
}

template <>
inline size_t u8<std::wstring_view>(char* out, size_t out_size, const std::wstring_view& str)
{
	return u8<std::u16string_view>(out, out_size, (const std::u16string_view&)str);
}

template <>
inline size_t u8<std::wstring>(char* out, size_t out_size, const std::wstring& str)
{
	return u8<std::u16string>(out, out_size, (const std::u16string&)str);
}

template <>
inline size_t u8<const wchar_t*>(char* out, size_t out_size, wchar_t const* const& str)
{
	return u8<std::u16string_view>(out, out_size, (const char16_t*)str);
}

template <typename T>
std::string u8(const T& val)
{
	size_t size = u8<T>(nullptr, 0, val);
	std::string result(size, '\0');
	u8<T>(result.data(), size, val);
	return result;
}

template <typename T>
size_t u16(char16_t* out, size_t out_size, const T& val)
{
	char tmp_buffer[32];

	if (auto r = std::to_chars(tmp_buffer, tmp_buffer + sizeof(tmp_buffer), val); r.ec != std::errc())
	{
		size_t size = r.ptr - out;

		if (size > out_size)
		{
			assert(false);
			return 0;
		}

		if (out)
		{
			for (size_t i = 0; i < size; ++i)
				out[i] = (char16_t)tmp_buffer[i];
		}

		return size;
	}

	assert(false);
	return 0;
}

template <>
inline size_t u16<std::string_view>(char16_t* out, size_t out_size, const std::string_view& view)
{
	assert(simdutf::validate_utf8(view.data(), view.size()));

	if (!out)
		return simdutf::utf16_length_from_utf8(view.data(), view.size());

	assert(out_size >= simdutf::utf16_length_from_utf8(view.data(), view.size()));
	return simdutf::convert_utf8_to_utf16le(view.data(), view.size(), out);
}

template <>
inline size_t u16<std::string>(char16_t* out, size_t out_size, const std::string& str)
{
	return u16<std::string_view>(out, out_size, str);
}

template <>
inline size_t u16<const char*>(char16_t* out, size_t out_size, char const* const& str)
{
	return u16<std::string_view>(out, out_size, str);
}

template <typename T>
std::u16string u16(const T& val)
{
	size_t size = u16<T>(nullptr, 0, val);
	std::u16string result(size, '\0');
	u16<T>(result.data(), size, val);
	return result;
}


template <typename TChar>
constexpr auto SplitView(const std::basic_string_view<TChar>& input, const std::basic_string_view<TChar>& delimiter)
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
std::vector<std::basic_string<TChar>> Split(const std::basic_string_view<TChar>& input, const std::basic_string_view<TChar>& delimiter)
{
	auto view = SplitView(input, delimiter);
	return std::vector(view.begin(), view.end());
}

} // namespace su
