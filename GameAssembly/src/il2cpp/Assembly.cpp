#include "pch.hpp"

#include "il2cpp/Assembly.hpp"

#include "il2cpp_data.hpp"

std::span<const il2cpp::Assembly*> il2cpp::Assembly::GetAssemblies()
{
	assert(g_il2cpp_data.GameAssembly);
	assert(g_il2cpp_data.il2cpp_domain_get_assemblies);

	size_t size = 0;
	auto arr = g_il2cpp_data.il2cpp_domain_get_assemblies(nullptr, &size);
	return std::span(arr, size);
}
