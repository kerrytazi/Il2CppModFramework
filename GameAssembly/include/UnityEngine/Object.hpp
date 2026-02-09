#pragma once

#include "System/Object.hpp"
#include "System/primitives.hpp"

namespace System { class String; }

namespace UnityEngine
{

enum class HideFlags;
struct Vector3;
struct Quaternion;
class Transform;

class __autogen Object : public System::Object
{
public:

	__autogen System::Int32 GetInstanceID();
	__autogen System::String* get_name();
	__autogen System::Void set_name(System::String* value);

	__autogen static UnityEngine::Object* Instantiate(UnityEngine::Object* original, UnityEngine::Vector3 position, UnityEngine::Quaternion rotation);
	__autogen static UnityEngine::Object* Instantiate(UnityEngine::Object* original, UnityEngine::Vector3 position, UnityEngine::Quaternion rotation, UnityEngine::Transform* parent);
	__autogen static UnityEngine::Object* Instantiate(UnityEngine::Object* original);
	__autogen static UnityEngine::Object* Instantiate(UnityEngine::Object* original, UnityEngine::Transform* parent);
	__autogen static UnityEngine::Object* Instantiate(UnityEngine::Object* original, UnityEngine::Transform* parent, System::Boolean instantiateInWorldSpace);

	__autogen static System::Void Destroy(UnityEngine::Object* obj, System::Single t);
	__autogen static System::Void Destroy(UnityEngine::Object* obj);
	__autogen static System::Void DestroyImmediate(UnityEngine::Object* obj, System::Boolean allowDestroyingAssets);
	__autogen static System::Void DestroyImmediate(UnityEngine::Object* obj);

	__autogen static System::Void DontDestroyOnLoad(UnityEngine::Object* obj);

	__autogen UnityEngine::HideFlags get_hideFlags();

	__autogen System::Void set_hideFlags(UnityEngine::HideFlags value);

	bool _IsValid() const
	{
		return (intptr_t)this != 0 && m_CachedPtr.val != 0;
	}

private:

	System::IntPtr m_CachedPtr;
};

} // namespace UnityEngine
