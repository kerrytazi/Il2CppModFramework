#include "pch.hpp"

#include "common/ModuleManager.hpp"
#include "common/Module.hpp"
#include "common/LazyStatic.hpp"

#include "common/Log.hpp"

#include "common/StringUtils.hpp"

#ifdef UC_ENABLE_IMGUI
#include "ImGuiHook.hpp"
#include "ImDrawDataCopy.hpp"
#endif // UC_ENABLE_IMGUI

struct RegisteredModule
{
	std::function<std::shared_ptr<Module>()> create;
	int priority = 0;
};

static LazyStatic<std::vector<RegisteredModule>> g_registered_modules;

void RegisterModule(std::function<std::shared_ptr<Module>()> create_module, int priority /*= 50*/)
{
	const auto proj = [](const RegisteredModule& rmodule) {
		return rmodule.priority;
	};

	auto it = std::ranges::lower_bound(*g_registered_modules, priority, std::less<>{}, proj);

	g_registered_modules->insert(it, RegisteredModule{
		.create = std::move(create_module),
		.priority = priority,
	});
}

ModuleManager::ModuleManager()
{
	modules_.reserve(g_registered_modules->size());

	for (const auto& rmodule : *g_registered_modules)
		modules_.push_back(rmodule.create());
}

ModuleManager::~ModuleManager()
{
	assert(load_state_ == ELoadState::Unloaded);
}

void ModuleManager::RequestLoad()
{
	Log::Debug("ModuleManager::RequestLoad");

	ELoadState check = ELoadState::Unloaded;
	if (!load_state_.compare_exchange_strong(check, ELoadState::DoLoad))
		throw std::runtime_error("ModuleManager::RequestLoad failed. Invalid state: " + std::to_string((int)check));
}

void ModuleManager::RequestUnload()
{
	Log::Debug("ModuleManager::RequestUnload");

	ELoadState check = ELoadState::Loaded;
	if (!load_state_.compare_exchange_strong(check, ELoadState::DoUnload))
	{
		if (check != ELoadState::DoUnload)
			throw std::runtime_error("ModuleManager::RequestUnload failed. Invalid state: " + std::to_string((int)check));
	}

	// Unload early.
	// Because we need to destroy detour on another thread.
	if (use_imgui_)
		UnloadImGui();
}

bool ModuleManager::IsUnloading() const
{
	return load_state_ == ELoadState::DoUnload;
}

bool ModuleManager::IsUnloaded() const
{
	return load_state_ == ELoadState::Unloaded;
}

#ifdef UC_ENABLE_IMGUI
void ModuleManager::RequestLoadImGui()
{
	assert(!use_imgui_);
	request_use_imgui_ = true;
}
#endif // UC_ENABLE_IMGUI

void ModuleManager::Load()
{
	Log::Debug("ModuleManager::Load");

	for (const auto& module : modules_)
		module->Load();
}

void ModuleManager::Unload()
{
	Log::Debug("ModuleManager::Unload");

	// Unload in reverse order to maintain priority.
	// The first load is the last unload.
	for (const auto& module : std::views::reverse(modules_))
		module->Unload();
}

void ModuleManager::PreUpdate()
{
	for (const auto& module : modules_)
		module->PreUpdate();
}

void ModuleManager::Update()
{
	for (const auto& module : modules_)
		module->Update();
}

void ModuleManager::PostUpdate()
{
	for (const auto& module : modules_)
		module->PostUpdate();
}

#pragma comment(linker, "/alternatename:GetConfigPath=DefaultGetConfigPath")
extern "C" const char* GetConfigPath();
extern "C" const char* DefaultGetConfigPath()
{
	return "client/config.json";
}

extern const std::string& GetExeDir();
static std::string g_config_path;

static const std::string& GetFullConfigPath()
{
	if (g_config_path.empty() && GetConfigPath() != nullptr)
		g_config_path = GetExeDir() + "/" + GetConfigPath();

	return g_config_path;
}

void ModuleManager::LoadConfig()
{
	auto path = GetFullConfigPath();

	if (path.empty())
		return;

	nlohmann::json doc;

	{
		std::ifstream file(path);
		doc = nlohmann::json::parse(file, nullptr, false);
	}

	if (doc.is_discarded())
		return;

	for (const auto& module : modules_)
		module->LoadConfig(doc);
}

void ModuleManager::SaveConfig()
{
	auto path = GetFullConfigPath();

	if (path.empty())
		return;

	auto path_tmp = std::string(path) + ".tmp";
	auto doc = nlohmann::json::object();

	for (const auto& module : modules_)
		module->SaveConfig(doc);

	{
		std::ofstream file(path_tmp);

		if (file.is_open())
			file << doc.dump(1, '\t');
	}

	std::filesystem::rename(path_tmp, path);
}

#ifdef UC_ENABLE_IMGUI
ImDrawDataCopy g_imgui_draw_data_renderer_copy;

void ModuleManager::OnImGui()
{
	if (!use_imgui_)
		return;

	bool render_imgui = ImGuiHook::IsClientMenuActive();

	if (!render_imgui)
	{
		for (const auto& module : modules_)
		{
			if (module->IsForceImGui())
			{
				render_imgui = true;
				break;
			}
		}
	}

	if (!render_imgui)
	{
		ImGuiHook::SetRenderActive(false);
		return;
	}

	if (ImGuiHook::IsFullyInitialized())
	{
		ImGuiHook::NewFrame();
		ImGui::NewFrame();

		if (ImGuiHook::IsClientMenuActive())
		{
			if (ImGui::IsKeyDown(ImGuiKey_LeftAlt) && ImGui::IsKeyPressed(ImGuiKey_Delete, false))
				RequestUnload();

			ImGui::SetNextWindowSizeConstraints(ImVec2(600, 200), ImVec2(1000, 1000));
			ImGui::Begin("Client Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar);

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Client"))
				{
					if (ImGui::MenuItem("Unload", "Alt+Delete"))
						RequestUnload();

					ImGui::EndMenu();
				}

				if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O) && std::filesystem::is_regular_file(GetConfigPath()))
					LoadConfig();

				if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S))
					SaveConfig();

				if (ImGui::BeginMenu("Config"))
				{
					if (ImGui::MenuItem("Load", "Ctrl+O", nullptr, std::filesystem::is_regular_file(GetConfigPath())))
						LoadConfig();

					if (ImGui::MenuItem("Save", "Ctrl+S"))
						SaveConfig();

					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			for (const auto& module : modules_)
				module->OnImGuiMenu();

			ImGui::End();
		}

		for (const auto& module : modules_)
			module->OnPostImGuiMenu();

		ImGui::EndFrame();
		ImGui::Render();

		g_imgui_draw_data_renderer_copy.CopyFrom(ImGui::GetDrawData());
	}

	ImGuiHook::SetRenderActive(true);
}

void ModuleManager::LoadImGui()
{
	assert(!use_imgui_);

	if (ImGuiHook::Load())
		use_imgui_ = true;
	else
		Log::Error("ModuleManager::LoadImGui() failed");
}

void ModuleManager::UnloadImGui()
{
	assert(use_imgui_);

	if (ImGuiHook::Unload())
		use_imgui_ = false;
	else
		Log::Error("ModuleManager::UnloadImGui() failed");
}
#endif // UC_ENABLE_IMGUI

extern void _Il2CppDisableUpdate();
extern void _BootstrapCleanup();
extern void _StartUnloadLibrary();

void _ModuleManagerCleanup()
{
	g_registered_modules = {};
}

void ModuleManager::OnPreUpdate()
{
	if (load_state_ == ELoadState::Unloaded)
		return;

	if (ELoadState check = ELoadState::DoLoad; load_state_.compare_exchange_strong(check, ELoadState::Loaded))
	{
		Load();

		if (load_state_ != ELoadState::DoUnload)
			LoadConfig();
		else
			Log::Warn(cs::Yellow("ModuleManager::RequestUnload was called during Load"));
	}

	if (load_state_ == ELoadState::DoUnload)
		return;

#ifdef UC_ENABLE_IMGUI
	if (bool check = true; request_use_imgui_.compare_exchange_strong(check, false))
		LoadImGui();
#endif // UC_ENABLE_IMGUI

	PreUpdate();
	Update();
}

void ModuleManager::OnPostUpdate()
{
	if (ELoadState check = ELoadState::DoUnload; load_state_.compare_exchange_strong(check, ELoadState::Unloaded))
	{
		Unload();
		SaveConfig();

		_Il2CppDisableUpdate();

#if defined(UC_LOADER_DLL_INJECTOR)
		_StartUnloadLibrary();
#elif defined(UC_LOADER_MANUAL_MAPPER)
		_BootstrapCleanup();
#else
#error Unknown Loader
#endif

		return;
	}

	PostUpdate();

#ifdef UC_ENABLE_IMGUI
	OnImGui();
#endif // UC_ENABLE_IMGUI
}
