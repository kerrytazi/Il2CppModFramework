#pragma once

#include "System/Object.hpp"
#include "il2cpp/Array.hpp"

namespace System
{

class String;

class Exception : Object
{
public:

	const String* GetMessage() const { return message; }
	const String* GetSource() const { return source; }
	const String* GetStackTrace() const { return stack_trace; }
	const Exception* GetInnerException() const { return inner_ex; }

private:

	using il2cpp_hresult_t = int32_t;

	String* className;
	String* message;
	Object* _data;
	Exception* inner_ex;
	String* _helpURL;
	System::Object* trace_ips; // Array?
	String* stack_trace;
	String* remote_stack_trace;
	int remote_stack_index;
	Object* _dynamicMethods;
	il2cpp_hresult_t hresult;
	String* source;
	Object* safeSerializationManager;
	System::Object* captured_traces; // Array?
	System::Object* native_trace_ips; // il2cpp::Array<System::IntPtr>
	int32_t caught_in_unmanaged;
};

} // namespace System

// Use this in catch
struct Il2CppExceptionWrapper
{
	System::Exception* ex;
};
