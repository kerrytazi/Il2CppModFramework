#pragma once

#include "common/NoImplement.hpp"
#include "common/autogen.hpp"

#include "il2cpp/Class.hpp"

namespace System
{

class String;

class __autogen Object : _NoImplement
{
public:

	__autogen_virtual System::String* ToString();

	const il2cpp::Class* GetClass() { return klass; }

	// Slow!!!
	// Consider caching method search via CallCached.
	template <typename TRet, typename... TArgs>
	TRet DynamicInvoke(std::string_view method_name, TArgs... args)
	{
		auto method = klass->FindMethodRecursive(method_name);
		assert(method);

		if (method->IsVirtual())
			method = klass->GetVirtualMethod(method);

		auto method_ptr = method->GetMethodPointer<TRet(System::Object*, std::remove_reference_t<TArgs>...)>();
		assert(method_ptr);

		return method_ptr(this, args...);
	}

private:

	const il2cpp::Class* klass;
	struct Il2CppMonitorData* monitor;
};

} // namespace System
