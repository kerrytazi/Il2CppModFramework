#pragma once

#include "System/Object.hpp"

#include <span>

namespace System
{

class Array : Object
{
public:

	size_t Length() const { return bounds ? bounds->length : max_length; }

	const void* _Data() const { return &first_val; }
	void* _Data() { return &first_val; }

	template <typename T>
	std::span<const T> AsSpan() const
	{
		return std::span((const T*)_Data(), Length());
	}

	template <typename T>
	std::span<T> AsSpan()
	{
		return std::span((T*)_Data(), Length());
	}

private:

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
