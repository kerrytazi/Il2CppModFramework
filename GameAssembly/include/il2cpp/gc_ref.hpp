#pragma once

#include <cassert>
#include <type_traits>

#include "il2cpp/il2cpp.hpp"

#include "UnityEngine/Object.hpp"

namespace il2cpp
{

template <typename T, bool Strong>
class gc_ref_base
{
public:

	using value_type = T;

	gc_ref_base() = default;

	template <typename T2>
	explicit gc_ref_base(T2* obj)
	{
		Create(obj);
	}

	template <typename T2, bool Strong2>
	gc_ref_base(const gc_ref_base<T2, Strong2>& other)
	{
		Create(other ? other.obj_ : nullptr);
	}

	template <typename T2, bool Strong2>
	gc_ref_base& operator=(const gc_ref_base<T2, Strong2>& other)
	{
		Destroy();
		Create(other ? other.obj_ : nullptr);
		return *this;
	}

	template <typename T2>
	gc_ref_base(gc_ref_base<T2, Strong>&& other)
	{
		swap(other);
	}

	template <typename T2>
	gc_ref_base(gc_ref_base<T2, !Strong>&& other)
	{
		Create(other ? other.obj_ : nullptr);
		other.Destroy();
	}

	template <typename T2>
	gc_ref_base& operator=(gc_ref_base<T2, Strong>&& other)
	{
		swap(other);
		return *this;
	}

	template <typename T2>
	gc_ref_base& operator=(gc_ref_base<T2, !Strong>&& other)
	{
		Create(other ? other.obj_ : nullptr);
		other.Destroy();
		return *this;
	}

	~gc_ref_base()
	{
		Destroy();
	}

	template <typename T2, bool Strong2>
	bool operator==(const gc_ref_base<T2, Strong2>& other) const
	{
		return obj_ == other.obj_;
	}

	T* operator->() const { assert(is_alive()); return obj_; }
	T& operator*() const { assert(is_alive()); return *obj_; }

	// Result of get_obj may be non-null but invalid after gc_ref_base destructor.
	// Consider using operators *, -> instead
	T* get_obj() const { return obj_; }

	operator bool() const { return is_alive(); }

	bool is_alive() const
	{
		if (!obj_)
			return false;

		if constexpr (!Strong)
			if (gchandle_get_target(gc_handle_) != obj_)
				return false;

		if (!CheckValidObj(obj_))
			return false;

		return true;
	}

	template <typename T2>
	constexpr void swap(gc_ref_base<T2, Strong>& other)
	{
		auto tmp_obj_ = other.obj_;
		auto tmp_gc_handle_ = other.gc_handle_;

		other.obj_ = static_cast<T2*>(obj_);
		other.gc_handle_ = gc_handle_;

		obj_ = static_cast<T*>(tmp_obj_);
		gc_handle_ = tmp_gc_handle_;
	}

private:

	template <typename T2, bool Strong2>
	friend class gc_ref_base;

	template <typename T2>
	static bool CheckValidObj(T2* obj)
	{
		if constexpr (std::is_base_of_v<UnityEngine::Object, T>)
		{
			if (!obj->_IsValid())
				return false;
		}
		else
		if constexpr (std::is_same_v<System::Object, T>)
		{
			if (auto uobj = obj->TryDownCast<UnityEngine::Object>())
				if (!uobj->_IsValid())
					return false;
		}

		return true;
	}

	template <typename T2> requires std::is_base_of_v<System::Object, T2>
	void Create(T2* obj)
	{
		if (obj && !CheckValidObj(obj))
			obj = nullptr;

		obj_ = static_cast<T*>(obj);

		if (obj)
		{
			if constexpr (Strong)
				gc_handle_ = gchandle_new(obj_, false);
			else
				gc_handle_ = gchandle_new_weakref(obj_, false);
		}
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

} // namespace il2cpp

template <typename T>
using gc_ref = il2cpp::gc_ref_base<T, true>;

template <typename T>
using gc_wref = il2cpp::gc_ref_base<T, false>;

template <typename T> requires std::is_base_of_v<System::Object, T>
gc_ref<T> make_ref(T* obj) { return gc_ref<T>(obj); }

template <typename T> requires std::is_base_of_v<System::Object, T>
gc_wref<T> make_wref(T* obj) { return gc_wref<T>(obj); }
