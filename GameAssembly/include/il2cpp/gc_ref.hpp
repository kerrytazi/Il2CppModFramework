#pragma once

#include <cassert>
#include <type_traits>

#include "il2cpp/il2cpp.hpp"

#include "UnityEngine/Object.hpp"

namespace il2cpp
{

// Weak reference for objects.
template <typename T>
class gc_ref
{
public:

	using value_type = T;

	gc_ref() = default;

	template <typename T2>
	explicit gc_ref(T2* obj)
	{
		Create(obj);
	}

	template <typename T2>
	gc_ref(const gc_ref<T2>& other)
	{
		Create(other.obj_);
	}

	template <typename T2>
	gc_ref& operator=(const gc_ref<T2>& other)
	{
		Destroy();
		Create(other.obj_);
		return *this;
	}

	template <typename T2>
	gc_ref(gc_ref<T2>&& other)
	{
		swap(other);
	}

	template <typename T2>
	gc_ref& operator=(gc_ref<T2>&& other)
	{
		swap(other);
		return *this;
	}

	~gc_ref()
	{
		Destroy();
	}

	template <typename T2>
	bool operator==(const gc_ref<T2>& other) const
	{
		return obj_ == other.obj_;
	}

	T* operator->() const { assert(is_alive()); return obj_; }
	T& operator*() const { assert(is_alive()); return *obj_; }

	// Result of get_obj may be non-null but invalid after gc_ref destructor.
	// Consider using operators *, -> instead
	T* get_obj() const { return obj_; }

	operator bool() const { return is_alive(); }

	bool is_alive() const
	{
		if (!obj_)
			return false;

		if (gchandle_get_target(gc_handle_) != obj_)
			return false;

		if constexpr (std::is_base_of_v<UnityEngine::Object, T>)
		{
			if (!obj_->_IsValid())
				return false;
		}
		else
		{
			if (auto uobj = obj_->TryDownCast<UnityEngine::Object>())
				if (!uobj->_IsValid())
					return false;
		}

		return true;
	}

	template <typename T2>
	constexpr void swap(gc_ref<T2>& other)
	{
		auto tmp_obj_ = other.obj_;
		auto tmp_gc_handle_ = other.gc_handle_;

		other.obj_ = static_cast<T2*>(obj_);
		other.gc_handle_ = gc_handle_;

		obj_ = static_cast<T*>(tmp_obj_);
		gc_handle_ = tmp_gc_handle_;
	}

private:

	template <typename T2>
	friend class gc_ref;

	template <typename T2>
	void Create(T2* obj)
	{
		obj_ = static_cast<T*>(obj);

		if (obj)
			gc_handle_ = gchandle_new_weakref(obj_, false);
	}

	void Destroy()
	{
		if (obj_)
			gchandle_free(gc_handle_);

#ifndef NDEBUG
		obj_ = (T*)0xCCCCCCCCCCCCCCCC;
		gc_handle_ = 0xCCCCCCCC;
#endif // NDEBUG
	}

	T* obj_ = nullptr;
	uint32_t gc_handle_ = 0;
};

template <typename T>
gc_ref(T*) -> gc_ref<T>;

} // namespace il2cpp
