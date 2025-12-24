#pragma once

#include "common/MyWindows.hpp"

#include <vector>
#include <unordered_map>
#include <string>
#include <string_view>

namespace System { class String; }

namespace il2cpp
{

class Domain;
class Class;
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

	std::vector<CachedClass> cached_classes;
	std::unordered_map<const il2cpp::Type*, std::string> cached_type_names;

	void Init();
};

inline MyIl2CppData g_il2cpp_data{};
