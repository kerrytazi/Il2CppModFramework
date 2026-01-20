#include "pch.hpp"

#include "il2cpp/ClassFinder.hpp"

#include "il2cpp/Class.hpp"
#include "il2cpp/Method.hpp"
#include "il2cpp/il2cpp.hpp"

#include <cassert>

#include "common/Templates.hpp"


template <typename TFunc, typename TFinder>
struct MethodFinderOnce
{
	using pointer = typename templates::func<TFunc>::pointer;
	static inline pointer ptr = nullptr;

	static pointer Find()
	{
		if (!ptr) [[unlikely]]
		{
			auto game_method = TFinder{}();
			assert(game_method);
			ptr = game_method->template GetMethodPointer<TFunc>();
			assert(ptr);
		}

		return ptr;
	}

	static pointer FindICall()
	{
		if (!ptr) [[unlikely]]
		{
			ptr = (pointer)TFinder{}();
			assert(ptr);
		}

		return ptr;
	}
};
