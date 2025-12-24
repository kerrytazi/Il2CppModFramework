#pragma once

#include "System/Object.hpp"

namespace System
{

class String;
class Array;

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
	Array* trace_ips;
	String* stack_trace;
	String* remote_stack_trace;
	int remote_stack_index;
	Object* _dynamicMethods;
	il2cpp_hresult_t hresult;
	String* source;
	Object* safeSerializationManager;
	Array* captured_traces;
	Array* native_trace_ips;
	int32_t caught_in_unmanaged;
};

} // namespace System

// Use this in catch
class Il2CppExceptionWrapper
{
public:

	System::Exception* GetException() { return ex; }

private:

	System::Exception* ex;
};
