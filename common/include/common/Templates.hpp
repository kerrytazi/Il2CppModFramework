#pragma once

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
};

} // namespace templates
