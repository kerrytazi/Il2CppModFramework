#pragma once

#include "common/autogen.hpp"
#include "System/primitives.hpp"
#include "UnityEngine/Object.hpp"

#include <ranges>

namespace System { class String; }

namespace UnityEngine
{

class Component;

class GameObject : public UnityEngine::Object
{
public:

	__autogen System::Int32 GetComponentCount();
	__autogen UnityEngine::Component* GetComponentAtIndex(System::Int32 index);

	__autogen static UnityEngine::GameObject* Find(System::String* name);

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
};

} // namespace UnityEngine
