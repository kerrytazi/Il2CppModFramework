#pragma once

#include "il2cpp/Method.hpp"
#include "common/Templates.hpp"

#include "il2cpp.hpp"

#include <cassert>

namespace il2cpp
{

template <typename TFunc, typename TFinder>
struct MethodFinderOnce
{
	using pointer = typename templates::func<TFunc>::pointer;
	static inline pointer ptr = nullptr;

	static pointer Find(TFinder finder)
	{
		if (!ptr) [[unlikely]]
		{
			auto game_method = finder();
			assert(game_method);
			ptr = game_method->template GetMethodPointer<TFunc>();
			assert(ptr);
		}

		return ptr;
	}

	static pointer FindICall(TFinder finder)
	{
		if (!ptr) [[unlikely]]
		{
			ptr = (pointer)finder();
			assert(ptr);
		}

		return ptr;
	}
};

template <typename TFunc, typename TFinder>
auto FindMethodOnce(TFinder finder)
{
	return MethodFinderOnce<TFunc, TFinder>::Find(finder);
}

template <typename TFunc, typename TFinder>
auto FindICallMethodOnce(TFinder finder)
{
	return MethodFinderOnce<TFunc, TFinder>::FindICall(finder);
}

} // namespace il2cpp
