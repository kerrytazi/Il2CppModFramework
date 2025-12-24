#pragma once

#include "nlohmann/json.hpp"

class Module
{
public:

	virtual ~Module();

	virtual void Load();
	virtual void Unload();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();

	virtual void SaveConfig(nlohmann::json& doc);
	virtual void LoadConfig(const nlohmann::json& doc);

#ifdef UC_ENABLE_IMGUI
	virtual bool IsForceImGui();
	virtual void OnImGuiMenu();
	virtual void OnPostImGuiMenu();
#endif // UC_ENABLE_IMGUI

	template <typename TConfig>
	void SimpleSaveConfig(nlohmann::json& doc, const TConfig& config)
	{
		doc[TConfig::mod_name] = config;
	}

	template <typename TConfig>
	const nlohmann::json* SimpleLoadConfig(const nlohmann::json& doc, TConfig& config)
	{
		auto mod = doc.find(TConfig::mod_name);

		if (mod != doc.end() && mod->is_object())
		{
			config = mod->get<TConfig>();
			return &*mod;
		}

		return nullptr;
	}
};
