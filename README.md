# Il2CppModFramework
## Framework for Unity games built with il2cpp.

# Contents
- [How to make my own mod](#how-to-make-my-own-mod)
- [Developing](#developing)
  - [Clone repo and submodules](#clone-repo-and-submodules)
  - [Build systems](#build-systems)
    - [CMake](#cmake)
      - [Configure](#configure)
      - [Compilation](#compilation)
    - [Visual Studio's “Open Folder” development](#visual-studios-open-folder-development)
  - [*(Optional)* Remove unnecessary folders from libs](#optional-remove-unnecessary-folders-from-libs)
- [CMake settings](#cmake-settings)
  - [Example](#example)
  - [Full list of options](#full-list-of-options)

# How to make my own mod
Complete steps in [Developing](#developing) section.

Copy `clients/example` to `clients/my-mod`.

`cd clients/my-mod`

*(Optional)* Init version control: `git init`

Edit `CMakeLists.txt`:
- `project("my_mod")`
- `set(UC_UNITY_VERSION 2021.3.56f2)`
- `config_client(my_mod)`

Edit `src/dllmain.cpp`:
- `GetConfigPath` - relative path *(from exe)* to mod settings.
- `GetUnityGameWindow` - expected name of game window.
- `GetImGuiIniFilename` - relative path *(from exe)* to imgui settings.
- `GetImGuiLogFilename` - relative path *(from exe)* to imgui logs.

You can remove these functions. Framework will fallback to default settings.

In `modules` folder *(or anywhere else inside `clients/my-mod` folder)* you can implement modules of your mod.

Each Module must be inherited from `Module`. And should be registered using `RegisterModuleStatic` for proper working.

**Note**: You can set loading order priority using `RegisterModuleStatic` second template argument.
Modules will `Unload` in reverse order. First to `Load` -> last to `Unload`.

Module can override any of:
- `virtual void Load()` - Called once module load.
- `virtual void Unload()` - Called once module unload.
- `virtual void PreUpdate()` - Called each frame *before* any other `MonoBehaviour` `Update`.
- `virtual void Update()` - Called each frame.
- `virtual void PostUpdate()` - Called each frame *after* any other `MonoBehaviour` `Update`.
- `virtual void SaveConfig(nlohmann::json& doc)` - Called when framework wants to save user settings *(usually before `Unload`)*.
- `virtual void LoadConfig(const nlohmann::json& doc)` - Called when framework wants to load user settings *(usually after `Load`)*.
- `virtual bool IsForceImGui()` - Return true if you want to render ImGui even if main menu is closed.
- `virtual void OnImGuiMenu()` - Called each frame when main menu visible. You should not create any new ImGui windows here.
- `virtual void OnPostImGuiMenu()` - Called each frame. You can create other ImGui windows here. Or render custom *onscreen* primitives.

**NOTE**: All functions are called on main thread.

**NOTE**: You can't use `UnityEngine::Input::GetKey` or similar input related functions in `PostUpdate`.

In `loggers` folder *(or anywhere else inside `clients/my-mod` folder)* you can implement custom loggers.
They are called each time you call any function from `Log` namespace.

Each Logger must be inherited from `Logger`. And should be registered using `RegisterLoggerStatic` for proper working.

**NOTE**: You can set log order priority using `RegisterLoggerStatic` second template argument.

Logger can override any of:
- `virtual void AddLine(Level level, Type type, std::initializer_list<cs::ColoredString> line)` - Called each time you call any function from `Log` namespace.
It's up to you to filter messages by `level` and `type`.

# Developing

## Clone repo and submodules
```sh
git clone https://github.com/kerrytazi/Il2CppModFramework.git
cd Il2CppFramework
git submodule update --init --filter=blob:none
```
`--filter=blob:none` because we don't need whole history for libraries. Which apparently downloads and uses ~500mb of extra storage.

## Build systems
### CMake
#### Configure
- `cmake -B build .`

#### Compilation
- `cmake --build build --target example_client --config Debug`
- `cmake --build build --target example_client --config Release`
- `cmake --build build --target example_client --config RelWithDebInfo`

Check out [CMake settings](#cmake-settings) for more info.

### Visual Studio's “Open Folder” development
- Open a local folder.
- Select `example_client` target and compile it.

## *(Optional)* Remove unnecessary folders from libs
- `libs/imgui/examples` because of [this bug](https://developercommunity.visualstudio.com/t/Visual-Studio-scans-for-sln-files-when/11026224).
- `libs/json/tools/` because Visual Studio searches for invalid `.natvis` files inside.

# CMake settings
You can pass additional options to cmake during configuration.

## Example:
- `cmake -B build . -DUC_ENABLE_IMGUI=OFF -DUC_LOADER=MANUAL_MAPPER`

## Full list of options
|Name|Description|Possible values. [Default]|
|-|-|-|
|`UC_ENABLE_IMGUI`|Enable ImGui|`[ON]/OFF`|
|`UC_LOADER`|Loader type|`[DLL_INJECTOR]/MANUAL_MAPPER`|


