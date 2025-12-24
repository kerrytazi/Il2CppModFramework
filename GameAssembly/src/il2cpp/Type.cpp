#include "pch.hpp"

#include "common/ExitScope.hpp"

#include "il2cpp/Type.hpp"

#include "il2cpp_data.hpp"

const std::string& il2cpp::Type::GetName() const
{
	assert(g_il2cpp_data.GameAssembly);
	assert(g_il2cpp_data.il2cpp_type_get_name);
	assert(g_il2cpp_data.il2cpp_free);

	auto& name = g_il2cpp_data.cached_type_names[this];

	if (name.empty())
	{
		auto ptr = g_il2cpp_data.il2cpp_type_get_name(this);
		const ExitScope delete_ptr([&]() { g_il2cpp_data.il2cpp_free(ptr); });
		name = ptr;
	}

	return name;
}

int il2cpp::Type::SizeOfType() const
{
	switch (type)
	{
		case Il2CppTypeEnum::Void: return 0;
		case Il2CppTypeEnum::Boolean: return 1;
		case Il2CppTypeEnum::Char: return 1;
		case Il2CppTypeEnum::I1: return 1;
		case Il2CppTypeEnum::U1: return 1;
		case Il2CppTypeEnum::I2: return 2;
		case Il2CppTypeEnum::U2: return 2;
		case Il2CppTypeEnum::I4: return 4;
		case Il2CppTypeEnum::U4: return 4;
		case Il2CppTypeEnum::I8: return 8;
		case Il2CppTypeEnum::U8: return 8;
		case Il2CppTypeEnum::R4: return 4;
		case Il2CppTypeEnum::R8: return 8;
		case Il2CppTypeEnum::String: return sizeof(void*);
		case Il2CppTypeEnum::Ptr: return sizeof(void*);
		case Il2CppTypeEnum::Class: return sizeof(void*);
		case Il2CppTypeEnum::I: return sizeof(void*);
		case Il2CppTypeEnum::U: return sizeof(void*);
		case Il2CppTypeEnum::FnPtr: return sizeof(void*);
		case Il2CppTypeEnum::Object: return sizeof(void*);
	}

	return -1;
}
