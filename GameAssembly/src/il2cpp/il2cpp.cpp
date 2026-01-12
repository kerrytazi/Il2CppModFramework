#include "pch.hpp"

#include "il2cpp/il2cpp.hpp"
#include "il2cpp/Class.hpp"
#include "il2cpp/Method.hpp"
#include "il2cpp/Assembly.hpp"
#include "il2cpp/Image.hpp"
#include "il2cpp/Type.hpp"

#include "common/Log.hpp"

#include "il2cpp_data.hpp"

static size_t GetTotalClassesCount()
{
	size_t total = 0;

	for (auto assembly : il2cpp::Assembly::GetImagesView())
		total += assembly->GetClassCount();

	return total;
}

static void Il2CppInitCaches()
{
	assert(g_il2cpp_data.il2cpp_class_get_methods);
	assert(g_il2cpp_data.il2cpp_class_get_fields);
	assert(g_il2cpp_data.il2cpp_class_get_properties);
	assert(g_il2cpp_data.il2cpp_class_get_events);

	Log::Debug("Il2CppInitCaches");

	g_il2cpp_data.cached_classes.reserve(GetTotalClassesCount());

	for (auto assembly : il2cpp::Assembly::GetImagesView())
	{
		for (auto klass : assembly->GetClassesView())
		{
			CachedClassFullName kfull_name{ klass->GetNamespace(), klass->GetName() };

			auto cit = std::ranges::lower_bound(g_il2cpp_data.cached_classes, kfull_name, std::less<>{});

			auto& cklass = *g_il2cpp_data.cached_classes.insert(cit, CachedClass{});
			cklass.namespaze = kfull_name.namespaze;
			cklass.name = kfull_name.name;
			cklass.klass = klass;

			// Call each get at least once to initialize them
			{ void* iter = nullptr; g_il2cpp_data.il2cpp_class_get_methods((il2cpp::Class*)klass, &iter); }
			{ void* iter = nullptr; g_il2cpp_data.il2cpp_class_get_fields((il2cpp::Class*)klass, &iter); }
			{ void* iter = nullptr; g_il2cpp_data.il2cpp_class_get_properties((il2cpp::Class*)klass, &iter); }
			{ void* iter = nullptr; g_il2cpp_data.il2cpp_class_get_events((il2cpp::Class*)klass, &iter); }
		}
	}
}

extern void _Il2CppPatchUpdate();

void _Il2CppLoad()
{
	Log::Debug("_Il2CppLoad");

	g_il2cpp_data.Init();
	Il2CppInitCaches();
	_Il2CppPatchUpdate();
}

void _Il2CppCleanCaches()
{
	g_il2cpp_data = {};
}

void* il2cpp::resolve_icall(const char* name)
{
	assert(g_il2cpp_data.GameAssembly);
	assert(g_il2cpp_data.il2cpp_resolve_icall);

	auto result = g_il2cpp_data.il2cpp_resolve_icall(name);

	if (!result)
		throw std::runtime_error(std::string("resolve_icall failed: ") + name);

	return result;
}

System::String* il2cpp::il2cpp_string_new(const char* str, int32_t len)
{
	assert(g_il2cpp_data.GameAssembly);
	assert(g_il2cpp_data.il2cpp_string_new);
	return g_il2cpp_data.il2cpp_string_new(str, len);
}

System::String* il2cpp::il2cpp_string_new_utf16(const char16_t* str, int32_t len)
{
	assert(g_il2cpp_data.GameAssembly);
	assert(g_il2cpp_data.il2cpp_string_new_utf16);
	return g_il2cpp_data.il2cpp_string_new_utf16(str, len);
}

intptr_t il2cpp::GetGameAssemblyBase()
{
	assert(g_il2cpp_data.GameAssembly);
	return (intptr_t)g_il2cpp_data.GameAssembly;
}

intptr_t il2cpp::GetUnityPlayerBase()
{
	assert(g_il2cpp_data.UnityPlayer);
	return (intptr_t)g_il2cpp_data.UnityPlayer;
}

intptr_t il2cpp::GetIDADllBaseAddress()
{
	return 0x180000000;
}
