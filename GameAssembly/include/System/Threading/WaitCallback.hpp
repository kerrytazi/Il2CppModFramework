#pragma once

#include "System/MulticastDelegate.hpp"

namespace System::Threading
{

class __autogen WaitCallback : System::MulticastDelegate
{
public:

	__autogen System::Void _ctor(System::Object* object, System::IntPtr method);

	[[nodiscard]]
	static WaitCallback* New(void(*func)());
};

} // namespace System::Threading
