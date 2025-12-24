#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/MyWindows.hpp"

std::string GetConfigPath()
{
	return "client/config.json";
}

std::string GetUnityGameWindowName()
{
	return "TestGame";
}

#ifdef UC_ENABLE_IMGUI
int GetImGuiSwitchClientMenuKey()
{
	return VK_INSERT;
}
#endif // UC_ENABLE_IMGUI

// FIXME: move to bootstrap
BOOL APIENTRY Bootstrap(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
BOOL APIENTRY DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	auto result = Bootstrap(hinstDLL, fdwReason, lpvReserved);

#ifdef UC_ENABLE_IMGUI
	if (fdwReason == DLL_PROCESS_ATTACH)
		g_module_manager->RequestLoadImGui();
#endif // UC_ENABLE_IMGUI

	return result;
}
