#include "pch.hpp"

#include "UnityEngine/Rendering/CommandBuffer.hpp"
#include "il2cpp/il2cpp.hpp"
#include "il2cpp/ClassFinder.hpp"

UnityEngine::Rendering::CommandBuffer* UnityEngine::Rendering::CommandBuffer::New()
{
	auto obj = (UnityEngine::Rendering::CommandBuffer*)il2cpp::object_new(il2cpp::Find<UnityEngine::Rendering::CommandBuffer>());
	obj->_ctor();
	return obj;
}
