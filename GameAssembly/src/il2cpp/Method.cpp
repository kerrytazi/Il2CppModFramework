#include "pch.hpp"
#include "il2cpp/Method.hpp"
#include "il2cpp/il2cpp.hpp"

uint16_t il2cpp::Method::GetVirtualMethodSlot() const
{
	assert(IsVirtual());
	return slot;
}

System::Reflection::MethodInfo* il2cpp::Method::ToReflectionMethod() const
{
	return (System::Reflection::MethodInfo*)il2cpp::method_get_object(this);
}
