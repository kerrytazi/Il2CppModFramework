#pragma once

#include <cstdint>

namespace System { class String; }
namespace System { class Object; }

namespace il2cpp
{

class Class;
class Type;
class Field;
class Method;

void* resolve_icall(const char* name);
System::Object* object_new(const il2cpp::Class* klass);
System::Object* array_new(const il2cpp::Class* element_klass, uintptr_t length);
System::String* string_new(const char* str, int32_t len);
System::String* string_new_utf16(const char16_t* str, int32_t len);
System::Object* type_get_object(const Type* type);
System::Object* method_get_object(const Method* method);
const il2cpp::Class* class_from_type(const Type* type);

const void field_static_get_value(const Field* field, void* out);
const void field_static_set_value(const Field* field, void* in);

uint32_t gchandle_new(System::Object* obj, bool pinned);
uint32_t gchandle_new_weakref(System::Object* obj, bool track_resurrection);
System::Object* gchandle_get_target(uint32_t gchandle);
void gchandle_free(uint32_t gchandle);

intptr_t GetGameAssemblyBase();
intptr_t GetUnityPlayerBase();
intptr_t GetIDADllBaseAddress();

} // namespace il2cpp
