#pragma once

#include "System/Object.hpp"
#include "il2cpp/ClassFinder.hpp"

#include <type_traits>

namespace il2cpp
{

System::Object* _ArrayNew(const il2cpp::Class* element_class, size_t size);

template <typename T>
class Array : System::Object
{
public:

	static Array<T>* New(size_t size)
	{
		return (Array<T>*)_ArrayNew(Find<std::remove_pointer_t<T>>(), size);
	}

	static Array<T>* New(std::initializer_list<T> values)
	{
		auto arr = New(values.size());

		for (size_t i = 0; i < values.size(); ++i)
			arr->at(i) = values.begin()[i];

		return arr;
	}

	size_t Length() const { return bounds ? bounds->length : max_length; }

	const void* Data() const { return &first_val; }
	void* Data() { return &first_val; }

	T& at(size_t index)
	{
		assert(index < Length());
		return begin()[index];
	}

	const T& at(size_t index) const
	{
		assert(index < Length());
		return begin()[index];
	}

	T& operator[](size_t index) { return at(index); }
	const T& operator[](size_t index) const { return at(index); }

	T* begin() { return (T*)Data(); }
	T* end() { return ((T*)Data()) + Length(); }

	const T* begin() const { return (const T*)Data(); }
	const T* end() const { return ((const T*)Data()) + Length(); }
	const T* cbegin() const { return (const T*)Data(); }
	const T* cend() const { return ((const T*)Data()) + Length(); }

protected:

	using il2cpp_array_size_t = uintptr_t;
	using il2cpp_array_lower_bound_t = int32_t;

	struct Il2CppArrayBounds
	{
		il2cpp_array_size_t length;
		il2cpp_array_lower_bound_t lower_bound;
	};

	/* bounds is NULL for szarrays */
	Il2CppArrayBounds* bounds;
	/* total number of elements of the array */
	il2cpp_array_size_t max_length;
	uint8_t first_val;
};

} // namespace System
