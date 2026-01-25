#include "pch.hpp"
#include "il2cpp/Method.hpp"

uint16_t il2cpp::Method::GetVirtualMethodSlot() const
{
	assert(IsVirtual());
	return slot;
}
