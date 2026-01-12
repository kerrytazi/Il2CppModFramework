#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/MyWindows.hpp"

// Optional. You can remove this function.
// Will fallback to DefaultGetConfigPath.
// If you want to disable config saving then return nullptr.
extern "C" const char* GetConfigPath()
{
	return "client/config.json";
}

// Optional. You can remove this function.
// Will fallback to DefaultGetUnityGameWindow.
extern "C" HWND GetUnityGameWindow()
{
	return FindWindowA(nullptr, "TestGame");
}

#ifdef UC_ENABLE_IMGUI
// Optional. You can remove this function.
// Will fallback to DefaultGetImGuiIniFilename.
// If you want to disable imgui saving then return nullptr.
extern "C" const char* GetImGuiIniFilename()
{
	return "config/imgui.ini";
}

// Optional. You can remove this function.
// Will fallback to DefaultGetImGuiLogFilename.
// If you want to disable imgui logs then return nullptr.
extern "C" const char* GetImGuiLogFilename()
{
	return "client/imgui.log";
}

// Optional. You can remove this function.
// Will fallback to DefaultGetImGuiSwitchClientMenuKey.
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
