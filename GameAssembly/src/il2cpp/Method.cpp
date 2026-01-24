#include "pch.hpp"
#include "il2cpp/Method.hpp"

#include "il2cpp/Class.hpp"
#include "il2cpp/Type.hpp"

#include "common/Log.hpp"

#include "il2cpp_data.hpp"

#include "il2cpp/Image.hpp"

uint16_t il2cpp::Method::GetVirtualMethodSlot() const
{
	assert(IsVirtual());
	return slot;
}

const il2cpp::Method* il2cpp::Method::Find(
	std::string_view namespaze,
	std::string_view class_name,
	std::string_view method_name,
	std::string_view ret_type,
	std::initializer_list<std::string_view> param_types,
	std::optional<bool> is_static /*= std::nullopt*/)
{
	if (auto klass = Class::Find(namespaze, class_name))
		return klass->FindMethod(method_name, ret_type, param_types, is_static);

	return nullptr;
}
