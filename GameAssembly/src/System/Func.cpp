#include "pch.hpp"

#include "System/Func.hpp"

#include "common/StringUtils.hpp"
#include "il2cpp/_weak_method_info.hpp"

#include <cassert>

static const il2cpp::Class* g_func_classes[16]{};

void* __CreateMethodInfoForFunc(const void* func, size_t parameters_count)
{
	auto method = _AllocMethodInfo();
	method->virtualMethodPointer = (il2cpp::TmpMethodPointer)func;
	method->parameters_count = (uint8_t)parameters_count;
	method->flags = 0x0010; // METHOD_ATTRIBUTE_STATIC
	return method;
}

const il2cpp::Class* __FindFuncClass(size_t templates_count)
{
	assert(templates_count < 16);
	auto& klass = g_func_classes[templates_count];

	if (!klass)
	{
		char buff[32];
		auto ptr = buff;
		auto end = buff + sizeof(buff);
		ptr += su::u8(ptr, end - ptr, "Func`");
		ptr += su::u8(ptr, end - ptr, templates_count);
		*ptr = '\0';

		klass = il2cpp::Class::Find("System", std::string_view(buff, ptr));
		assert(klass);
	}

	return klass;
}
