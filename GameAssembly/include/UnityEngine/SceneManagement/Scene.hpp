#pragma once

#include "common/autogen.hpp"
#include "System/primitives.hpp"

#include <ranges>

namespace il2cpp { template <typename T> class Array; }
namespace UnityEngine { class GameObject; }
namespace System { class String; }

namespace UnityEngine::SceneManagement
{

struct __autogen Scene
{
	__autogen System::Boolean IsValid();
	__autogen System::String* get_name();
	__autogen System::Boolean get_isLoaded();

	__autogen il2cpp::Array<UnityEngine::GameObject*>* GetRootGameObjects();

	int m_Handle;
};

} // namespace UnityEngine
