#pragma once

#include <cstdint>

namespace System { class String; }
namespace System { class Object; }

namespace il2cpp
{

void* resolve_icall(const char* name);
System::String* string_new(const char* str, int32_t len);
System::String* string_new_utf16(const char16_t* str, int32_t len);

uint32_t gchandle_new(System::Object* obj, bool pinned);
uint32_t gchandle_new_weakref(System::Object* obj, bool track_resurrection);
System::Object* gchandle_get_target(uint32_t gchandle);
void gchandle_free(uint32_t gchandle);

intptr_t GetGameAssemblyBase();
intptr_t GetUnityPlayerBase();
intptr_t GetIDADllBaseAddress();

} // namespace il2cpp
