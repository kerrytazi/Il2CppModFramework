# Il2CppModFramework
## A framework for creating mods for Unity games built with il2cpp.

# Contents
- [How to make my own mod](#how-to-make-my-own-mod)
- [Developing](#developing)
  - [Clone repo and submodules](#clone-repo-and-submodules)
  - [Build systems](#build-systems)
    - [CMake](#cmake)
      - [Configure](#configure)
      - [Compilation](#compilation)
    - [Visual Studio](#visual-studio)
- [CMake settings](#cmake-settings)
  - [Example](#example)
  - [Full list of options](#full-list-of-options)

# How to make my own mod
Follow the steps in the [Developing](#developing) section.

Copy the `clients/example` to `clients/my-mod`.

Then, navigate to it: `cd clients/my-mod`

*(Optional)* Init version control: `git init`

Edit the `CMakeLists.txt`:
- `project("my_mod")`
- `set(UC_UNITY_VERSION 2021.3.56f2)`
- `config_client(my_mod)`

Edit the `src/dllmain.cpp`:
- `GetConfigPath` - relative path *(from exe)* to mod settings.
- `GetUnityGameWindow` - expected name of game window.
- `GetImGuiIniFilename` - relative path *(from exe)* to imgui settings.
- `GetImGuiLogFilename` - relative path *(from exe)* to imgui logs.

You can remove these functions. The framework will fallback to default settings.

In `modules` folder *(or anywhere else inside `clients/my-mod` folder)* you can implement modules of your mod.

Each Module must inherit from `Module` class and should be registered using `RegisterModuleStatic` to work properly.

**Note**: You can set the loading order priority using the second template argument of `RegisterModuleStatic`.
Modules will `Unload` in reverse order. The first to `Load` is the last to `Unload`.

Module can override any of:
- `virtual void Load()` - Called once module load.
- `virtual void Unload()` - Called once module unload.
- `virtual void PreUpdate()` - Called each frame *before* any other `MonoBehaviour`'s `Update`.
- `virtual void Update()` - Called each frame.
- `virtual void PostUpdate()` - Called each frame *after* any other `MonoBehaviour`'s `Update`.
- `virtual void SaveConfig(nlohmann::json& doc)` - Called when the framework wants to save user settings *(usually before `Unload`)*.
- `virtual void LoadConfig(const nlohmann::json& doc)` - Called when the framework wants to load user settings *(usually after `Load`)*.
- `virtual bool IsForceImGui()` - Return true if you want to render ImGui even if main menu is closed.
- `virtual void OnImGuiMenu()` - Called each frame when main menu visible. You should not create any new ImGui windows here.
- `virtual void OnPostImGuiMenu()` - Called each frame. You can create other ImGui windows here. Or render custom *onscreen* primitives.

**NOTE**: All functions are called on main thread.

**NOTE**: You can't use `UnityEngine::Input::GetKey` or similar input related functions in `PostUpdate`.

In the `loggers` folder *(or anywhere else inside `clients/my-mod` folder)* you can implement custom loggers.
These are called each time you call any function from the `Log` namespace.

Each Logger must be inherited from `Logger`. And should be registered using `RegisterLoggerStatic` to work properly.

**NOTE**: You can set log order priority using `RegisterLoggerStatic` second template argument.

Logger can override any of:
- `virtual void AddLine(Level level, Type type, std::initializer_list<cs::StyledString> line)` - Called each time you call any function from the `Log` namespace.
- `virtual void Flush()` - Flush any cached info. Usually called after `Unload` or during crash handler.
It's up to you to filter messages by their `level` and `type`.

# Developing

## Clone repo and submodules
```sh
git clone https://github.com/kerrytazi/Il2CppModFramework.git
cd Il2CppModFramework
git submodule update --init --filter=blob:none
```
`--filter=blob:none` is used because we don't need whole history for libraries, which would otherwise download and use ~500MB of extra storage.

## Build systems
### CMake
#### Configure
- `cmake -B build .`

#### Compilation
- `cmake --build build --target example_client --config Debug`
- `cmake --build build --target example_client --config Release`
- `cmake --build build --target example_client --config RelWithDebInfo`

See [CMake settings](#cmake-settings) for more information.

### Visual Studio
- Open a Visual Studio -> Get started -> Open a local folder
- Select the `example_client` target and compile it.
- *(Optional)* Remove `libs/imgui/examples` due to [this bug](https://developercommunity.visualstudio.com/t/Visual-Studio-scans-for-sln-files-when/11026224).
- *(Optional)* Remove `libs/json/tools/` because Visual Studio searches for invalid `.natvis` files inside it.

**NOTE**: When you add/remove files from your project/framework you need to regenerate cmake. You can do it by clicking Project -> Delete Cache and Reconfigure.

# CMake settings
You can pass additional options to cmake during configuration.

## Example:
- `cmake -B build . -DUC_ENABLE_IMGUI=OFF -DUC_LOADER=MANUAL_MAPPER`

## Full list of options
|Name|Description|Possible values. [Default]|
|-|-|-|
|`UC_ENABLE_IMGUI`|Enable ImGui|`[ON]/OFF`|
|`UC_LOADER`|Loader type|`[DLL_INJECTOR]/MANUAL_MAPPER`|
|`UC_STACK_TRACER`|Stack Tracer implementation|`[DBGHELP]/DIA/NONE`|
