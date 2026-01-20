#pragma once

#include "System/Object.hpp"

#include "UnityEngine/SceneManagement/Scene.hpp"

namespace UnityEngine::SceneManagement
{

class __autogen SceneManager : public System::Object
{
public:

	__autogen static System::Int32 get_sceneCount();

	__autogen static UnityEngine::SceneManagement::Scene GetActiveScene();
	__autogen static UnityEngine::SceneManagement::Scene GetSceneByName(System::String* name);
	__autogen static UnityEngine::SceneManagement::Scene GetSceneAt(System::Int32 index);
};

} // namespace UnityEngine
