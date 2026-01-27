#include "pch.hpp"

#include "UnityEngine/Events/UnityAction.hpp"
#include "il2cpp/il2cpp.hpp"
#include "il2cpp/ClassFinder.hpp"
#include "il2cpp/_weak_method_info.hpp"

UnityEngine::Events::UnityAction* UnityEngine::Events::UnityAction::New(void(*func)())
{
	auto method = _AllocMethodInfo();
	method->virtualMethodPointer = func;
	method->parameters_count = 0;
	method->flags = 0x0010; // METHOD_ATTRIBUTE_STATIC
	auto obj = (UnityEngine::Events::UnityAction*)il2cpp::object_new(il2cpp::Find<UnityEngine::Events::UnityAction>());
	obj->_ctor(nullptr, std::bit_cast<System::IntPtr>(&method));
	__RegisterTmpMethodInfo(obj, method);
	return obj;
}
