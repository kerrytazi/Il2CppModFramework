#include "pch.hpp"

#include "System/String.hpp"
#include "common/StringUtils.hpp"

#include "il2cpp/il2cpp.hpp"

System::String* System::String::New(const std::string_view& str)
{
	return il2cpp::il2cpp_string_new(str.data(), (int32_t)str.size());
}

System::String* System::String::New(const std::u16string_view& str)
{
	return il2cpp::il2cpp_string_new_utf16(str.data(), (int32_t)str.size());
}

std::string System::String::AsString() const
{
	if ((uintptr_t)this == 0)
		return "(null)";

	return su::u8(AsU16StringView());
}
