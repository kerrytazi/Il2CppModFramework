#pragma once

#include <cstdint>

namespace System { class String; }

namespace il2cpp
{

void* resolve_icall(const char* name);
System::String* il2cpp_string_new(const char* str, int32_t len);
System::String* il2cpp_string_new_utf16(const char16_t* str, int32_t len);

intptr_t GetGameAssemblyBase();
intptr_t GetUnityPlayerBase();
intptr_t GetIDADllBaseAddress();

} // namespace il2cpp
