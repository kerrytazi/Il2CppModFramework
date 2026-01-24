#include "pch.hpp"

#include "il2cpp_data.hpp"

#include "common/Log.hpp"

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
			_name = reinterpret_cast<decltype(_name)>(GetProcAddress(GameAssembly, #_name)); \
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
	init_func(il2cpp_class_from_type);

	init_func(il2cpp_gchandle_new);
	init_func(il2cpp_gchandle_new_weakref);
	init_func(il2cpp_gchandle_get_target);
	init_func(il2cpp_gchandle_free);

	init_func(il2cpp_class_get_method_from_name);
	init_func(il2cpp_init);
	init_func(il2cpp_domain_get);
	init_func(il2cpp_assembly_get_image);
	init_func(il2cpp_class_get_methods);
	init_func(il2cpp_class_get_fields);
	init_func(il2cpp_class_get_properties);
	init_func(il2cpp_class_get_events);
	init_func(il2cpp_class_get_type);
	#undef init_func

	cached_type_names.reserve(16 * 1024);
}

