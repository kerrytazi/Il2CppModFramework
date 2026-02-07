#include "pch.hpp"

#include "il2cpp/Method.hpp"
#include "il2cpp/il2cpp.hpp"

#include "System/Exception.hpp"
#include "System/String.hpp"

#include "common/Log.hpp"

uint16_t il2cpp::Method::GetVirtualMethodSlot() const
{
	assert(IsVirtual());
	return slot;
}

System::Reflection::MethodInfo* il2cpp::Method::ToReflectionMethod() const
{
	return (System::Reflection::MethodInfo*)il2cpp::method_get_object(this);
}

void il2cpp::Method::_LogRuntimeInvoke(System::Exception* exc) const
{
	Log::Error(cs::Red("RuntimeInvoke Exception: "), cs::Red(exc->GetMessage()->AsU16StringView()));
}

void* il2cpp::Method::_ObjectUnbox(System::Object* obj)
{
	return obj->Unbox();
}
