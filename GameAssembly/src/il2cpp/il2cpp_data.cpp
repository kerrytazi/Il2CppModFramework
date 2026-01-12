#include "pch.hpp"

#include "il2cpp_data.hpp"

#include "common/Log.hpp"

#include <format>

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

	Log::Debug("MyIl2CppData::Init GameAssembly: " + std::format("{:#016x}", (intptr_t)GameAssembly));
	Log::Debug("MyIl2CppData::Init UnityPlayer: " + std::format("{:#016x}", (intptr_t)UnityPlayer));

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
	init_func(il2cpp_string_new);
	init_func(il2cpp_class_get_methods);
	init_func(il2cpp_class_get_fields);
	init_func(il2cpp_class_get_properties);
	init_func(il2cpp_class_get_events);
	#undef init_func

	cached_type_names.reserve(16 * 1024);
}

