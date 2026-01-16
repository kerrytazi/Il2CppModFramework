#ifdef UC_ENABLE_IMGUI
#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/Module.hpp"

#include "UnityEngine/Input.hpp"

#include "imgui.h"
extern void ImGui::ShowDemoWindow(bool*);

class ImGuiDemoModule : public Module
{
public:

	virtual void Update() override
	{
		if (UnityEngine::Input::GetKeyDown(UnityEngine::KeyCode::P) &&
			UnityEngine::Input::GetKey(UnityEngine::KeyCode::LeftAlt))
		{
			show_window_ = true;
		}
	}

	virtual bool IsForceImGui() override
	{
		return show_window_;
	}

	virtual void OnPostImGuiMenu() override
	{
		if (!show_window_)
			return;

		ImGui::ShowDemoWindow(&show_window_);
	}

private:

	bool show_window_ = false;
};

static RegisterModuleStatic<ImGuiDemoModule> registered;

#endif // UC_ENABLE_IMGUI
