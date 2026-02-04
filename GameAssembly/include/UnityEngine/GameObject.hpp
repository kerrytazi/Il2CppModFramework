#pragma once

#include "System/primitives.hpp"
#include "System/Reflection/MethodInfo.hpp"
#include "UnityEngine/Object.hpp"
#include "common/CallCached.hpp"
#include "il2cpp/Array.hpp"

#include <ranges>

namespace System { class String; }
namespace UnityEngine::SceneManagement { struct Scene; }

namespace UnityEngine
{

class Component;
class Transform;

class __autogen GameObject : public UnityEngine::Object
{
public:

#if ICMF_UNITY_VERSION_NUM >= 2022308945
	__autogen System::Int32 GetComponentCount();
	__autogen UnityEngine::Component* GetComponentAtIndex(System::Int32 index);
#endif // ICMF_UNITY_VERSION_NUM >= 2022308945

	__autogen System::Void SetActive(System::Boolean value);
	__autogen System::Boolean get_activeSelf();

	__autogen UnityEngine::Transform* get_transform();

	__autogen System::Int32 get_layer();
	__autogen System::Void set_layer(System::Int32 value);

	__autogen UnityEngine::SceneManagement::Scene get_scene();

	__autogen static UnityEngine::GameObject* Find(System::String* name);

	__autogen il2cpp::Array<UnityEngine::Component*>* GetComponents(System::Type* type);

	// TODO: Benchmark against GetComponentsView
	il2cpp::Array<UnityEngine::Component*>* GetComponents()
	{
		auto comp_ref_type = CallCached([]() {
			return il2cpp::Find<UnityEngine::Component>()->GetType()->ToReflectionType();
		});

		return GetComponents(comp_ref_type);
	}

#if ICMF_UNITY_VERSION_NUM >= 2022308945
	template <typename T = void>
	auto GetComponentsView()
	{
		auto get_component = [this](System::Int32 index) {
			return GetComponentAtIndex(index);
		};

		return
			std::views::iota(System::Int32(0), GetComponentCount()) |
			std::views::transform(get_component);
	}
#endif // ICMF_UNITY_VERSION_NUM >= 2022308945
};

} // namespace UnityEngine
