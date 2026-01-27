#pragma once

#include "System/MulticastDelegate.hpp"

namespace UnityEngine::Events
{

class __autogen UnityAction : public System::MulticastDelegate
{
public:

	__autogen System::Void _ctor(System::Object* object, System::IntPtr method);

	[[nodiscard]]
	static UnityAction* New(void(*func)());
};

} // namespace System
