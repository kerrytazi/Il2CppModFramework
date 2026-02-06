#pragma once

#include "System/Object.hpp"

#include <string>
#include <vector>
#include <string_view>

namespace System
{

class __autogen String : public System::Object
{
public:

	[[nodiscard]]
	static String* New(const std::string_view& str);
	[[nodiscard]]
	static String* New(const std::u16string_view& str);

	template <size_t N>
	[[nodiscard]]
	static String* New(const char (&str)[N])
	{
		return New(std::string_view(str, N - 1));
	}

	template <size_t N>
	[[nodiscard]]
	static String* New(const char16_t (&str)[N])
	{
		return New(std::u16string_view(str, N - 1));
	}

	std::u16string_view AsU16StringView() const
	{
		if ((uintptr_t)this == 0)
			return u"(null)";

		return std::u16string_view((const char16_t*)(&firstChar), length);
	}

	std::string AsString() const;

	// UNSAFE. Result stored statically and will be invalidated on next call.
	// null-terminated
	// No thread safety.
	// Consider using AsU16StringView or AsString instead.
	// Unless you absolutely understand what you are doing.
	std::string_view AsStringView() const;

private:

	int32_t length;
	wchar_t firstChar;
};

} // namespace System

[[nodiscard]]
inline System::String* operator""_il2(const char* str, size_t size)
{
	return System::String::New({ str, size });
}

[[nodiscard]]
inline System::String* operator""_il2(const char16_t* str, size_t size)
{
	return System::String::New({ str, size });
}
