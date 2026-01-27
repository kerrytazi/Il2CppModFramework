#pragma once

#include "System/primitives.hpp"
#include "UnityEngine/Component.hpp"

#include <ranges>

namespace UnityEngine
{

class GameObject;
struct Vector3;
struct Quaternion;
struct Matrix4x4;

class __autogen Transform : public UnityEngine::Component
{
public:

	__autogen UnityEngine::Vector3 get_position();
	__autogen UnityEngine::Vector3 get_localPosition();
	__autogen UnityEngine::Vector3 get_eulerAngles();
	__autogen UnityEngine::Vector3 get_localEulerAngles();
	__autogen UnityEngine::Vector3 get_right();
	__autogen UnityEngine::Vector3 get_up();
	__autogen UnityEngine::Vector3 get_forward();
	__autogen UnityEngine::Quaternion get_rotation();
	__autogen UnityEngine::Quaternion get_localRotation();
	__autogen UnityEngine::Vector3 get_localScale();
	__autogen UnityEngine::Transform* get_parent();

	__autogen System::Void set_position(UnityEngine::Vector3 val);
	__autogen System::Void set_localPosition(UnityEngine::Vector3 val);
	__autogen System::Void set_eulerAngles(UnityEngine::Vector3 val);
	__autogen System::Void set_localEulerAngles(UnityEngine::Vector3 val);
#if UC_UNITY_VERSION_NUM >= 2022308945
	__autogen System::Void set_right(UnityEngine::Vector3 val);
	__autogen System::Void set_up(UnityEngine::Vector3 val);
	__autogen System::Void set_forward(UnityEngine::Vector3 val);
#endif // UC_UNITY_VERSION_NUM >= 2022308945
	__autogen System::Void set_rotation(UnityEngine::Quaternion val);
	__autogen System::Void set_localRotation(UnityEngine::Quaternion val);
	__autogen System::Void set_localScale(UnityEngine::Vector3 val);
	__autogen System::Void set_parent(UnityEngine::Transform* val);

	__autogen UnityEngine::Matrix4x4 get_worldToLocalMatrix();
	__autogen UnityEngine::Matrix4x4 get_localToWorldMatrix();

	__autogen UnityEngine::Transform* get_root();

	__autogen System::Int32 get_childCount();

	__autogen UnityEngine::Transform* GetChild(System::Int32 index);

	auto GetChildrenView()
	{
		auto get_child = [this](System::Int32 index) {
			return GetChild(index);
		};

		return
			std::views::iota(System::Int32(0), get_childCount()) |
			std::views::transform(get_child);
	}

	auto GetGameObjectsView()
	{
		auto get_child = [this](UnityEngine::Transform* transform) {
			return transform->get_gameObject();
		};

		return
			GetChildrenView() |
			std::views::transform(get_child);
	}
};

} // namespace UnityEngine
