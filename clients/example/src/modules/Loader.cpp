#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/Module.hpp"

#include "common/Log.hpp"

#include "UnityEngine/Input.hpp"

#include "UnityEngine/Application.hpp"
#include "System/Func.hpp"
#include "System/String.hpp"

class LoaderModule : public Module
{
public:

	virtual void Load() override
	{
		_CallCachedEverything();

#if UC_UNITY_VERSION_NUM >= 2022308945
		wants_to_quit_.emplace([this]() {
			Log::Debug("Application::wantsToQuit");
			is_quiting_ = true;
			g_module_manager->RequestUnload();
			return true;
		});

		wants_to_quit_func_ = System::Func<System::Boolean>::New(wants_to_quit_->get_static_pointer());

		UnityEngine::Application::add_wantsToQuit(wants_to_quit_func_);

		Log::Debug("Game Version: ", UnityEngine::Application::get_version()->AsU16StringView());
		Log::Debug("Unity Version: ", UnityEngine::Application::get_unityVersion()->AsU16StringView());
#endif // UC_UNITY_VERSION_NUM >= 2022308945
	}

#if UC_UNITY_VERSION_NUM >= 2022308945
	virtual void Unload() override
	{
		UnityEngine::Application::remove_wantsToQuit(wants_to_quit_func_);

		if (is_quiting_)
			UnityEngine::Application::Quit();
	}
#endif // UC_UNITY_VERSION_NUM >= 2022308945

	virtual void Update() override
	{
		if (UnityEngine::Input::GetKeyDown(UnityEngine::KeyCode::Delete) &&
			UnityEngine::Input::GetKey(UnityEngine::KeyCode::LeftAlt))
		{
			g_module_manager->RequestUnload();
		}
	}

private:

#if UC_UNITY_VERSION_NUM >= 2022308945
	std::optional<sl::lambda<bool()>> wants_to_quit_;
	System::Func<System::Boolean>* wants_to_quit_func_ = nullptr;
	bool is_quiting_ = false;
#endif // UC_UNITY_VERSION_NUM >= 2022308945
};

static RegisterModuleStatic<LoaderModule> registered;
