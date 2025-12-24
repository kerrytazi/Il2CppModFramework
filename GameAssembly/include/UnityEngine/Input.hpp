#pragma once

#include "common/autogen.hpp"
#include "System/primitives.hpp"
#include "System/Object.hpp"
#include "UnityEngine/KeyCode.hpp"

namespace UnityEngine
{

class Input : public System::Object
{
public:

	__autogen static System::Boolean GetKey(UnityEngine::KeyCode key);
	__autogen static System::Boolean GetKeyUp(UnityEngine::KeyCode key);
	__autogen static System::Boolean GetKeyDown(UnityEngine::KeyCode key);
};

} // namespace UnityEngine
