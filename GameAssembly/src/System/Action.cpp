#include "pch.hpp"

#include "System/Func.hpp"
#include "il2cpp/_tmp_method_info.hpp"

#include "common/StringUtils.hpp"
#include <cassert>

// TODO: Cleanup?
static std::vector<std::unique_ptr<il2cpp::TmpMethodInfo>> g_method_infos_for_action;

void* __CreateMethodInfoForAction(const void* func, size_t parameters_count)
{
	auto method = g_method_infos_for_action.emplace_back(std::make_unique<il2cpp::TmpMethodInfo>()).get();
	method->name = "";
	method->klass = (il2cpp::Class*)CallCached<decltype([]() { return il2cpp::Class::Find("", "<>c"); })>();
	method->virtualMethodPointer = (il2cpp::TmpMethodPointer)func;
	method->parameters_count = (uint8_t)parameters_count;
	method->flags = 0x0010; // METHOD_ATTRIBUTE_STATIC
	return method;
}

const il2cpp::Class* __FindActionClass(size_t templates_count)
{
	char buff[32];
	auto ptr = buff;
	auto end = buff + sizeof(buff);
	ptr += su::u8(ptr, end - ptr, "Action");

	if (templates_count > 0)
	{
		*ptr++ = '`';
		ptr += su::u8(ptr, end - ptr, templates_count);
	}

	*ptr = '\0';

	auto klass = il2cpp::Class::Find("System", std::string_view(buff, ptr));
	assert(klass);
	return klass;
}
