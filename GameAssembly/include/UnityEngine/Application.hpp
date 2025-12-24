#pragma once

#include "common/autogen.hpp"
#include "System/Object.hpp"
#include "System/primitives.hpp"

namespace UnityEngine
{

class Application : public System::Object
{
public:

	__autogen static System::Boolean get_isFocused();
	__autogen static System::Int32 get_targetFrameRate();
	__autogen static System::Void set_targetFrameRate(System::Int32 val);
};

} // namespace UnityEngine
