#pragma once

#include "System/Object.hpp"
#include "System/primitives.hpp"

namespace System { template <typename... TArgs> class Action; }
namespace System { template <typename... TArgsAndRet> class Func; }

namespace UnityEngine
{

namespace Events { class UnityAction; }

class __autogen Application : public System::Object
{
public:

	__autogen static System::Void Quit();
	__autogen static System::Boolean get_isFocused();
#if UC_UNITY_VERSION_NUM >= 2022308945
	__autogen static System::String* get_buildGUID();
#endif // UC_UNITY_VERSION_NUM >= 2022308945
	__autogen static System::Boolean get_runInBackground();
	__autogen static System::Void set_runInBackground(System::Boolean value);
	__autogen static System::Boolean get_isBatchMode();
	__autogen static System::String* get_dataPath();
	__autogen static System::String* get_streamingAssetsPath();
	__autogen static System::String* get_persistentDataPath();
	__autogen static System::String* get_temporaryCachePath();

#if UC_UNITY_VERSION_NUM >= 2022308945
	__autogen static System::String* get_unityVersion();
	__autogen static System::String* get_version();
#endif // UC_UNITY_VERSION_NUM >= 2022308945

	__autogen static System::Int32 get_targetFrameRate();
	__autogen static System::Void set_targetFrameRate(System::Int32 val);

#if UC_UNITY_VERSION_NUM >= 2022308945
	__autogen static System::Void add_onBeforeRender(UnityEngine::Events::UnityAction* value);
	__autogen static System::Void remove_onBeforeRender(UnityEngine::Events::UnityAction* value);
#endif // UC_UNITY_VERSION_NUM >= 2022308945
	__autogen static System::Void add_focusChanged(System::Action<System::Boolean>* value);
	__autogen static System::Void remove_focusChanged(System::Action<System::Boolean>* value);
#if UC_UNITY_VERSION_NUM >= 2022308945
	__autogen static System::Void add_wantsToQuit(System::Func<System::Boolean>* value);
	__autogen static System::Void remove_wantsToQuit(System::Func<System::Boolean>* value);
#endif // UC_UNITY_VERSION_NUM >= 2022308945
	__autogen static System::Void add_quitting(System::Action<>* value);
	__autogen static System::Void remove_quitting(System::Action<>* value);
};

} // namespace UnityEngine
