#pragma once

#include "common/MyWindows.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>

namespace System { class String; }
namespace System { class Object; }

namespace il2cpp
{

class Domain;
class Class;
class Method;
class Field;
class Property;
class Event;
class Assembly;
class Image;
class Type;

} // namespace il2cpp

struct CachedClassFullName
{
	std::string_view namespaze;
	std::string_view name;

	constexpr std::strong_ordering operator<=>(const CachedClassFullName& other) const = default;
};

struct CachedClass : CachedClassFullName
{
	const il2cpp::Class* klass;
};

struct MyIl2CppData
{
	HINSTANCE GameAssembly;
	HINSTANCE UnityPlayer;

	const il2cpp::Assembly** (*il2cpp_domain_get_assemblies)(const il2cpp::Domain* domain, size_t* size);
	const il2cpp::Class* (*il2cpp_image_get_class)(const il2cpp::Image* image, size_t index);
	void* (*il2cpp_resolve_icall)(const char* name);
	char* (*il2cpp_type_get_name)(const il2cpp::Type* type);
	void (*il2cpp_free)(void* ptr);
	System::String* (*il2cpp_string_new)(const char* str, int32_t len);
	System::String* (*il2cpp_string_new_utf16)(const char16_t* str, int32_t len);

	uint32_t (*il2cpp_gchandle_new)(System::Object* obj, bool pinned);
	uint32_t (*il2cpp_gchandle_new_weakref)(System::Object* obj, bool track_resurrection);
	System::Object* (*il2cpp_gchandle_get_target)(uint32_t gchandle);
	void (*il2cpp_gchandle_free)(uint32_t gchandle);

	const il2cpp::Method* (*il2cpp_class_get_method_from_name)(il2cpp::Class* klass, const char* name, int argsCount);
	int (*il2cpp_init)(const char* domain_name);
	il2cpp::Domain* (*il2cpp_domain_get)();
	const il2cpp::Image* (*il2cpp_assembly_get_image)(const il2cpp::Assembly* assembly);
	il2cpp::Method* (*il2cpp_class_get_methods)(il2cpp::Class* klass, void** iter);
	il2cpp::Field* (*il2cpp_class_get_fields)(il2cpp::Class* klass, void** iter);
	il2cpp::Property* (*il2cpp_class_get_properties)(il2cpp::Class* klass, void** iter);
	il2cpp::Event* (*il2cpp_class_get_events)(il2cpp::Class* klass, void** iter);
	il2cpp::Class* (*il2cpp_class_from_type)(const il2cpp::Type* type);
	const il2cpp::Type* (*il2cpp_class_get_type)(il2cpp::Class* klass);

	std::vector<CachedClass> cached_classes;
	std::unordered_map<const il2cpp::Type*, std::string> cached_type_names;

	void Init();
};

inline MyIl2CppData g_il2cpp_data{};
