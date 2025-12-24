#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/Module.hpp"

#include "common/Log.hpp"

#include "UnityEngine/Input.hpp"

class LoaderModule : public Module
{
public:

	virtual void Update() override
	{
		if (UnityEngine::Input::GetKeyDown(UnityEngine::KeyCode::Delete) &&
			UnityEngine::Input::GetKey(UnityEngine::KeyCode::LeftAlt))
		{
			g_module_manager->RequestUnload();
		}
	}

private:

	bool imgui_active_ = false;
};

static RegisterModuleStatic<LoaderModule> registered;
