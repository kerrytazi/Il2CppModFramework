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
	return _AsString();
}

void System::String::AsString(std::string& cache) const
{
	if ((uintptr_t)this == 0)
	{
		cache = "(null)";
		return;
	}

	auto u16_str = AsU16StringView();
	cache.resize(u16_str.size() * 2 + 1); // allocate more
	auto result_size = su::u8(cache.data(), cache.capacity(), u16_str);

	assert(result_size >= u16_str.size());
	cache.resize(result_size); // shrink to correct size
}

static std::string g_null_str = "(null)";
static std::string g_as_string_cache;
const std::string& System::String::_AsString() const
{
	if ((uintptr_t)this == 0)
		return g_null_str;

	AsString(g_as_string_cache);
	return g_as_string_cache;
}
