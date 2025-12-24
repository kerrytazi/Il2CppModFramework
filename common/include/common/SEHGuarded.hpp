#pragma once

#include <type_traits>

template <typename TFunc, typename TRet = std::invoke_result_t<TFunc>>
std::optional<TRet> SEHGuarded(TFunc&& func)
{
	__try
	{
		return func();
	}
	__except (1) // EXCEPTION_EXECUTE_HANDLER
	{
		return std::nullopt;
	}
}
