#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/Module.hpp"

#include "UnityEngine/Application.hpp"

#ifdef UC_ENABLE_IMGUI
#include "imgui.h"
#endif // UC_ENABLE_IMGUI

struct FPSLimiterConfig
{
	bool enable = false;
	System::Int32 limited_fps = 20;

	static constexpr std::string_view mod_name = "fps_limiter";
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(FPSLimiterConfig, enable, limited_fps);

class FPSLimiterModule : public Module
{
public:

	virtual void Unload()
	{
		CheckLimit(true);
	}

	virtual void Update() override
	{
		CheckLimit(UnityEngine::Application::get_isFocused());
	}

	virtual void SaveConfig(nlohmann::json& doc) override
	{
		SimpleSaveConfig(doc, config_);
	}

	virtual void LoadConfig(const nlohmann::json& doc) override
	{
		SimpleLoadConfig(doc, config_);
	}

#ifdef UC_ENABLE_IMGUI
	virtual void OnImGuiMenu() override
	{
		if (ImGui::CollapsingHeader("FPS Limiter"))
		{
			ImGui::Checkbox("Enable", &config_.enable);
			ImGui::InputInt("Unfocused FPS", &config_.limited_fps);
		}
	}
#endif // UC_ENABLE_IMGUI

private:

	void CheckLimit(bool is_focused)
	{
		if (!config_.enable)
			return;

		if (limited_ != is_focused)
			return;

		if (limited_)
		{
			limited_ = false;
			UnityEngine::Application::set_targetFrameRate(normal_fps_);
		}
		else
		{
			limited_ = true;
			normal_fps_ = UnityEngine::Application::get_targetFrameRate();
			UnityEngine::Application::set_targetFrameRate(config_.limited_fps);
		}
	}

	FPSLimiterConfig config_;
	System::Int32 normal_fps_ = 60;
	bool limited_ = false;
};

static RegisterModuleStatic<FPSLimiterModule> registered;
