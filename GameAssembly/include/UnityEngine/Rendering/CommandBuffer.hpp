#pragma once

#include "UnityEngine/Object.hpp"

namespace UnityEngine::Rendering
{

class __autogen CommandBuffer : public UnityEngine::Object
{
public:

	__autogen System::Void _ctor();
	__autogen System::Void Clear();

	[[nodiscard]]
	__autogen_new static UnityEngine::Rendering::CommandBuffer* New();

private:

	System::IntPtr m_Ptr;
};

} // namespace UnityEngine
