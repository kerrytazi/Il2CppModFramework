#pragma once

#include <cstddef>
#include <cstdint>
#include <cassert>
#include <string>
#include <vector>
#include <array>
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

class UTF8Iterator
{
public:

	using value_type = uint32_t;
	using difference_type = ptrdiff_t;

	explicit UTF8Iterator(const char* ptr)
		: p{ ptr }
	{
		Recalc();
	}

	UTF8Iterator()
		: UTF8Iterator(nullptr)
	{
	}

	UTF8Iterator& operator++()
	{
		p += char_size;
		Recalc();
		return *this;
	}

	UTF8Iterator operator++(int)
	{
		UTF8Iterator result(*this);
		++(*this);
		return result;
	}

	value_type operator*() const
	{
		return unicode_val;
	}

	bool operator==(const UTF8Iterator& other) const { return p == other.p; }

private:

	void Recalc()
	{
		if (!p || p[0] == '\0')
		{
#ifndef NDEBUG
			char_size = 1;
			unicode_val = 0;
#endif // !NDEBUG
			return;
		}

		switch (char_size = CHAR_SIZES[(uint8_t)p[0]])
		{
			[[unlikely]] case 0:
				char_size = 1;
				unicode_val = 0xFFFD;
				break;

			case 1:
				unicode_val = uint32_t(p[0]);
				break;

			case 2:
				unicode_val = ((uint32_t(p[0]) & 0x1F) << 6) | (uint32_t(p[1]) & 0x3F);
				break;

			case 3:
				unicode_val = ((uint32_t(p[0]) & 0x0F) << 12) | ((uint32_t(p[1]) & 0x3F) << 6) | (uint32_t(p[2]) & 0x3F);
				break;

			case 4:
				unicode_val = ((uint32_t(p[0]) & 0x07) << 18) | ((uint32_t(p[1]) & 0x3F) << 12) | ((uint32_t(p[2]) & 0x3F) << 6) | (uint32_t(p[3]) & 0x3F);
				break;
		}
	}

	const char* p;
	uint32_t char_size;
	uint32_t unicode_val;

	// ASCII: 0xxxxxxx
	// 2-byte: 110xxxxx 10xxxxxx
	// 3-byte: 1110xxxx 10xxxxxx 10xxxxxx
	// 4-byte: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	static constexpr std::array<uint8_t, 256> CHAR_SIZES = ([]() {
		std::array<uint8_t, 256> result{};

		// 1-byte UTF-8: 0xxxxxxx (ASCII)
		for (int i = 0; i < 128; ++i)
			result[i] = 1;

		// Continuation bytes: 10xxxxxx
		// These are never the first byte of a character
		for (int i = 0x80; i < 0xC0; ++i)      // 128-191
			result[i] = 0;  // Invalid as first byte

		// 2-byte UTF-8 first byte: 110xxxxx (192-223)
		for (int i = 0xC0; i < 0xE0; ++i)      // 192-223
			result[i] = 2;

		// 3-byte UTF-8 first byte: 1110xxxx (224-239)
		for (int i = 0xE0; i < 0xF0; ++i)      // 224-239
			result[i] = 3;

		// 4-byte UTF-8 first byte: 11110xxx (240-247)
		for (int i = 0xF0; i < 0xF8; ++i)      // 240-247
			result[i] = 4;

		// Invalid UTF-8 start bytes: 11111xxx (248-255)
		for (int i = 0xF8; i < 256; ++i)       // 248-255
			result[i] = 0;  // Invalid

		return result;
	})();
};

template <typename TIt>
class UnicodeIterator
{
public:

	using value_type = std::string_view;
	using difference_type = typename std::iterator_traits<TIt>::difference_type;

	explicit UnicodeIterator(TIt _it)
		: it{ _it }
	{
	}

	UnicodeIterator()
		: UnicodeIterator(TIt{})
	{
	}

	UnicodeIterator& operator++()
	{
		++it;
		return *this;
	}

	UnicodeIterator operator++(int)
	{
		UnicodeIterator result(*this);
		++(*this);
		return result;
	}

	value_type operator*() const
	{
		const_cast<UnicodeIterator*>(this)->Recalc();
		return Convert();
	}

	bool operator==(const UnicodeIterator& other) const { return it == other.it; }

private:

	std::string_view Convert() const
	{
		return std::string_view(buf, buf + char_size);
	}

	void Recalc()
	{
		uint32_t uni = *it;

		if (uni <= 0x7F)
		{
			char_size = 1;
			buf[0] = (char)uni;
		}
		else
		if (uni <= 0x7FF)
		{
			// 2-byte UTF-8: 110xxxxx 10xxxxxx
			char_size = 2;
			buf[0] = (char)(0xC0 | (uni >> 6));
			buf[1] = (char)(0x80 | (uni & 0x3F));
		}
		else
		if (uni <= 0xFFFF)
		{
			// 3-byte UTF-8: 1110xxxx 10xxxxxx 10xxxxxx
			char_size = 3;
			buf[0] = (char)(0xE0 | (uni >> 12));
			buf[1] = (char)(0x80 | ((uni >> 6) & 0x3F));
			buf[2] = (char)(0x80 | (uni & 0x3F));
		}
		else
		if (uni <= 0x10FFFF)
		{
			// 4-byte UTF-8: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
			char_size = 4;
			buf[0] = (char)(0xF0 | (uni >> 18));
			buf[1] = (char)(0x80 | ((uni >> 12) & 0x3F));
			buf[2] = (char)(0x80 | ((uni >> 6) & 0x3F));
			buf[3] = (char)(0x80 | (uni & 0x3F));
		}
		else
		{
			// Invalid Unicode code point (beyond maximum valid value 0x10FFFF)
			char_size = 0;
		}

		buf[char_size] = '\0';
	}

	TIt it;
	uint8_t char_size;
	char buf[7];
};

class UTF8StringView
{
public:

	UTF8StringView(std::string_view _sv)
		: sv{ _sv }
	{
	}

	UTF8Iterator begin() const { return UTF8Iterator(sv.data()); }
	UTF8Iterator end() const { return UTF8Iterator(sv.data() + sv.size()); }

private:

	std::string_view sv;
};

} // namespace su


namespace std::ranges
{

template <>
constexpr bool enable_borrowed_range<su::UTF8StringView> = true;

} // std::ranges

static_assert(std::forward_iterator<su::UTF8Iterator>, "forward_iterator<su::UTF8Iterator>");
static_assert(std::sentinel_for<su::UTF8Iterator, su::UTF8Iterator>, "sentinel_for<su::UTF8Iterator, su::UTF8Iterator>");

static_assert(std::ranges::forward_range<su::UTF8StringView>, "forward_range<su::UTF8StringView>");
static_assert(std::ranges::borrowed_range<su::UTF8StringView>, "borrowed_range<su::UTF8StringView>");

