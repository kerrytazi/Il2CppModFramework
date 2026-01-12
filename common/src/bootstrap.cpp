#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/LoggerManager.hpp"

#include "common/Log.hpp"

#include "common/MyWindows.hpp"

#include <format>

void _Il2CppLoad();

static std::unique_ptr<ModuleManager> g_module_manager_storage;
static std::unique_ptr<LoggerManager> g_logger_manager_storage;

static HMODULE g_dll_instance = nullptr;

extern void _Il2CppCleanCaches();
extern void _ModuleManagerCleanup();
extern void _LoggerCleanup();

void _BootstrapCleanup()
{
	Log::Debug("_BootstrapCleanup");

	g_module_manager = nullptr;
	g_logger_manager = nullptr;

	_ModuleManagerCleanup();
	_LoggerCleanup();
}

void _UnloadLibraryFromThread()
{
	assert(g_dll_instance);

	Log::Debug("_UnloadLibraryFromThread");

	// NOTE: _UnloadLibraryFromThread is supposed to be called from a new thread.
	// Each new thread increments library ref count,
	// so we need to call FreeLibrary one more time.
	FreeLibrary(g_dll_instance);
	FreeLibraryAndExitThread(g_dll_instance, 0);
}

static std::string g_exe_path;
static std::string g_exe_dir;

const std::string& GetExePath() { return g_exe_path; }
const std::string& GetExeDir() { return g_exe_dir; }

static void InitExePath()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);
	g_exe_path = buffer;
	g_exe_dir = std::filesystem::path(g_exe_path).parent_path().string();
}

BOOL APIENTRY Bootstrap(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH:
			InitExePath();
			g_dll_instance = hinstDLL;

			g_logger_manager_storage = std::make_unique<LoggerManager>();
			g_logger_manager = g_logger_manager_storage.get();

			Log::Debug("Bootstrap DLL_PROCESS_ATTACH " + std::format("{:#016x}", (intptr_t)hinstDLL));

			g_module_manager_storage = std::make_unique<ModuleManager>();
			g_module_manager = g_module_manager_storage.get();

			_Il2CppLoad();
			break;

		case DLL_THREAD_ATTACH:
			Log::Debug("Bootstrap DLL_THREAD_ATTACH");
			break;

		case DLL_THREAD_DETACH:
			Log::Debug("Bootstrap DLL_THREAD_DETACH");
			break;

		case DLL_PROCESS_DETACH:
			Log::Debug("Bootstrap DLL_PROCESS_DETACH");

			_BootstrapCleanup();
			break;
	}

	return TRUE;
}
