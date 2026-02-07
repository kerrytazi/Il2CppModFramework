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
	assert(g_il2cpp_data.GameAssembly);

	Log::Debug("Il2CppInitCaches");

	// Causes long loading and rarely needed.
	// Use Class::_ForceInit*() before using Class.
	// g_il2cpp_data.il2cpp_init("IL2CPP Root Domain");

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

System::Object* il2cpp::object_new(const Class* klass)
{
	assert(g_il2cpp_data.il2cpp_object_new);
	return g_il2cpp_data.il2cpp_object_new(klass);
}

System::Object* il2cpp::array_new(const Class* element_klass, uintptr_t length)
{
	assert(g_il2cpp_data.il2cpp_array_new);
	return g_il2cpp_data.il2cpp_array_new(element_klass, length);
}

uint32_t il2cpp::gchandle_new(System::Object* obj, bool pinned)
{
	assert(g_il2cpp_data.il2cpp_gchandle_new);
	return g_il2cpp_data.il2cpp_gchandle_new(obj, pinned);
}

uint32_t il2cpp::gchandle_new_weakref(System::Object* obj, bool track_resurrection)
{
	assert(g_il2cpp_data.il2cpp_gchandle_new_weakref);
	return g_il2cpp_data.il2cpp_gchandle_new_weakref(obj, track_resurrection);
}

System::Object* il2cpp::gchandle_get_target(uint32_t gchandle)
{
	assert(g_il2cpp_data.il2cpp_gchandle_get_target);
	return g_il2cpp_data.il2cpp_gchandle_get_target(gchandle);
}

void il2cpp::gchandle_free(uint32_t gchandle)
{
	assert(g_il2cpp_data.il2cpp_gchandle_free);
	return g_il2cpp_data.il2cpp_gchandle_free(gchandle);
}

System::String* il2cpp::string_new(const char* str, int32_t len)
{
	assert(g_il2cpp_data.il2cpp_string_new);
	return g_il2cpp_data.il2cpp_string_new(str, len);
}

System::String* il2cpp::string_new_utf16(const char16_t* str, int32_t len)
{
	assert(g_il2cpp_data.il2cpp_string_new_utf16);
	return g_il2cpp_data.il2cpp_string_new_utf16(str, len);
}

System::Object* il2cpp::type_get_object(const Type* type)
{
	assert(g_il2cpp_data.il2cpp_type_get_object);
	return g_il2cpp_data.il2cpp_type_get_object(type);
}

System::Object* il2cpp::method_get_object(const Method* method)
{
	assert(g_il2cpp_data.il2cpp_method_get_object);
	return g_il2cpp_data.il2cpp_method_get_object(method);
}

System::Object* il2cpp::runtime_invoke(const Method* method, void* obj, void** params, System::Exception** exc)
{
	assert(g_il2cpp_data.il2cpp_runtime_invoke);
	return g_il2cpp_data.il2cpp_runtime_invoke(method, obj, params, exc);
}

const il2cpp::Class* il2cpp::class_from_type(const Type* type)
{
	assert(g_il2cpp_data.il2cpp_class_from_type);
	return g_il2cpp_data.il2cpp_class_from_type(type);
}

const void il2cpp::field_static_get_value(const Field* field, void* out)
{
	assert(g_il2cpp_data.il2cpp_field_static_get_value);
	return g_il2cpp_data.il2cpp_field_static_get_value(field, out);
}

const void il2cpp::field_static_set_value(const Field* field, void* in)
{
	assert(g_il2cpp_data.il2cpp_field_static_set_value);
	return g_il2cpp_data.il2cpp_field_static_set_value(field, in);
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
