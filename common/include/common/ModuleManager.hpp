#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <atomic>

class Module;

void RegisterModule(std::function<std::shared_ptr<Module>()> create_module, int priority = 50);

template <typename T, int priority = 50>
struct RegisterModuleStatic
{
	inline static bool registered = ([](){
		RegisterModule([]() -> std::shared_ptr<Module> {
			return std::make_unique<T>();
		}, priority);

		return true;
	})();
};

class ModuleManager
{
public:

	ModuleManager();
	~ModuleManager();

	void RequestLoad();
	void RequestUnload();
	bool IsUnloading() const;
	bool IsUnloaded() const;

#ifdef UC_ENABLE_IMGUI
	void RequestLoadImGui();
#endif // UC_ENABLE_IMGUI

private:

	std::vector<std::shared_ptr<Module>> modules_;

	enum class ELoadState
	{
		Unloaded,
		DoLoad,
		Loaded,
		DoUnload,
	};

	std::atomic<ELoadState> load_state_;

	void Load();
	void Unload();
	void PreUpdate();
	void Update();
	void PostUpdate();

	void LoadConfig();
	void SaveConfig();

#ifdef UC_ENABLE_IMGUI
	std::atomic<bool> request_use_imgui_ = false;
	std::atomic<bool> use_imgui_ = false;

	void OnImGui();

	void LoadImGui();
	void UnloadImGui();
#endif // UC_ENABLE_IMGUI

	friend struct FriendIl2CppOnUpdate;
	void OnPreUpdate();
	void OnPostUpdate();
};

inline ModuleManager* g_module_manager = nullptr;
