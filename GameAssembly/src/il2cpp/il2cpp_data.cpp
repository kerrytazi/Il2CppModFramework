#include "pch.hpp"

#include "il2cpp_data.hpp"

#include "common/Log.hpp"

#pragma comment(linker, "/alternatename:GetIl2CppFunction=DefaultGetIl2CppFunction")
extern "C" const void* GetIl2CppFunction(const void* GameAssemblyLibrary, const char* name);
extern "C" const void* DefaultGetIl2CppFunction(const void* GameAssemblyLibrary, const char* name)
{
	return GetProcAddress((HINSTANCE)GameAssemblyLibrary, name);
}

void MyIl2CppData::Init()
{
	if (GameAssembly)
	{
		assert("il2cpp::Init() called twice" && false);
		return;
	}

	GameAssembly = GetModuleHandle("GameAssembly.dll");
	UnityPlayer = GetModuleHandle("UnityPlayer.dll");

	if (!GameAssembly)
		throw std::runtime_error("Can't load GameAssembly.dll");

	Log::Debug("MyIl2CppData::Init GameAssembly: ", GameAssembly);
	Log::Debug("MyIl2CppData::Init UnityPlayer: ", UnityPlayer);

	#define init_func(_name) \
		do \
		{ \
			_name = reinterpret_cast<decltype(_name)>(GetIl2CppFunction(GameAssembly, #_name)); \
			if (!_name) \
				throw std::runtime_error("Can't get proc address: " #_name); \
		} \
		while (false)

	init_func(il2cpp_domain_get_assemblies);
	init_func(il2cpp_image_get_class);
	init_func(il2cpp_resolve_icall);
	init_func(il2cpp_type_get_name);
	init_func(il2cpp_free);
	init_func(il2cpp_object_new);
	init_func(il2cpp_array_new);
	init_func(il2cpp_string_new);
	init_func(il2cpp_string_new_utf16);
	init_func(il2cpp_type_get_object);
	init_func(il2cpp_method_get_object);
	init_func(il2cpp_runtime_invoke);
	init_func(il2cpp_class_from_type);

	init_func(il2cpp_field_static_get_value);
	init_func(il2cpp_field_static_set_value);

	init_func(il2cpp_gchandle_new);
	init_func(il2cpp_gchandle_new_weakref);
	init_func(il2cpp_gchandle_get_target);
	init_func(il2cpp_gchandle_free);

	init_func(il2cpp_class_get_methods);
	init_func(il2cpp_class_get_fields);
	init_func(il2cpp_class_get_properties);
	init_func(il2cpp_class_get_events);

	#undef init_func

	cached_type_names.reserve(16 * 1024);
}

