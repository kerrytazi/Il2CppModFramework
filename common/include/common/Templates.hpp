#pragma once

#include <tuple>

namespace templates
{

template <typename T>
static constexpr bool always_false = false;

template <typename TFunc>
struct func
{
	static_assert(always_false<TFunc>, "Invalid function declaration. Try something like this: <int(int, int)>");
};

template <typename TRet, typename... TArgs>
struct func<TRet(TArgs...)>
{
	using pointer = TRet(*)(TArgs...);
	using args = std::tuple<TArgs...>;
	using ret_type = TRet;
	static constexpr size_t arity = sizeof...(TArgs);
};

} // namespace templates
