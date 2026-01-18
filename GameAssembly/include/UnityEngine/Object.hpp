#pragma once

#include "System/Object.hpp"
#include "System/primitives.hpp"

namespace System { class String; }

namespace UnityEngine
{

class Object : public System::Object
{
public:

	__autogen System::Int32 GetInstanceID();
	__autogen System::String* get_name();
	__autogen System::Void set_name(System::String* value);
	__autogen static System::Void Destroy(UnityEngine::Object* obj);
	__autogen static System::Void DontDestroyOnLoad(UnityEngine::Object* obj);

	bool _IsValid()
	{
		return (intptr_t)this != 0 && m_CachedPtr != 0;
	}

private:

	System::IntPtr m_CachedPtr;
};

} // namespace UnityEngine
