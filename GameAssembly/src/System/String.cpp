#include "pch.hpp"

#include "System/String.hpp"
#include "common/StringUtils.hpp"

#include "il2cpp/il2cpp.hpp"

System::String* System::String::New(const std::string_view& str)
{
	return il2cpp::string_new(str.data(), (int32_t)str.size());
}

System::String* System::String::New(const std::u16string_view& str)
{
	return il2cpp::string_new_utf16(str.data(), (int32_t)str.size());
}

std::string System::String::AsString() const
{
	if ((uintptr_t)this == 0)
		return "(null)";

	return su::u8(AsU16StringView());
}

static std::vector<char> g_as_string_cache;

std::string_view System::String::AsStringView() const
{
	if ((uintptr_t)this == 0)
		return "(null)";

	auto u16_str = AsU16StringView();
	g_as_string_cache.reserve(u16_str.size() * 2 + 1);
	auto result_size = su::u8(g_as_string_cache.data(), g_as_string_cache.capacity(), u16_str);

	assert(result_size >= u16_str.size());
	g_as_string_cache.data()[result_size] = '\0';
	return std::string_view(g_as_string_cache.data(), result_size);
}
