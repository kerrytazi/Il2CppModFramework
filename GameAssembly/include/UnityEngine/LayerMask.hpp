#pragma once

#include "common/autogen.hpp"

namespace il2cpp { template <typename T> class Array; }
namespace System { class String; }

namespace UnityEngine
{

struct __autogen LayerMask
{
	int m_Mask;

	__autogen static System::String* LayerToName(System::Int32 layer);
	__autogen static System::Int32 NameToLayer(System::String* layerName);
	__autogen static System::Int32 GetMask(il2cpp::Array<System::String*>* layerNames);
};

} // namespace UnityEngine
