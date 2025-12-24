#include "pch.hpp"

#include "common/Module.hpp"

Module::~Module()
{
}

void Module::Load()
{
}

void Module::Unload()
{
}

void Module::PreUpdate()
{
}

void Module::Update()
{
}

void Module::PostUpdate()
{
}

void Module::SaveConfig(nlohmann::json& doc)
{
}

void Module::LoadConfig(const nlohmann::json& doc)
{
}

#ifdef UC_ENABLE_IMGUI
bool Module::IsForceImGui()
{
	return false;
}

void Module::OnImGuiMenu()
{
}

void Module::OnPostImGuiMenu()
{
}
#endif // UC_ENABLE_IMGUI
