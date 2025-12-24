#pragma once

#include "common/autogen.hpp"
#include "System/primitives.hpp"
#include "System/Object.hpp"
#include "UnityEngine/Rendering/GraphicsDeviceType.hpp"

namespace UnityEngine
{

class SystemInfo : public System::Object
{
public:

	__autogen_icall static UnityEngine::Rendering::GraphicsDeviceType GetGraphicsDeviceType();
};

} // namespace UnityEngine
