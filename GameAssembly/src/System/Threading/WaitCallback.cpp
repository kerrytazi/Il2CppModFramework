#include "pch.hpp"

#include "System/Threading/WaitCallback.hpp"
#include "il2cpp/il2cpp.hpp"
#include "il2cpp/ClassFinder.hpp"
#include "il2cpp/_weak_method_info.hpp"

System::Threading::WaitCallback* System::Threading::WaitCallback::New(void(*func)())
{
	auto method = _AllocMethodInfo();
	method->virtualMethodPointer = func;
	method->parameters_count = 0;
	method->flags = 0x0010; // METHOD_ATTRIBUTE_STATIC
	auto obj = (WaitCallback*)il2cpp::object_new(il2cpp::Find<WaitCallback>());
	obj->_ctor(nullptr, System::IntPtr{ (intptr_t)&method });
	__RegisterTmpMethodInfo(obj, method);
	return obj;
}
