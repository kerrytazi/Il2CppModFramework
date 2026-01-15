#pragma once

#include "common/Log.hpp"

#include <string_view>
#include <type_traits>

template <typename TFunc, typename TRet = std::invoke_result_t<TFunc>>
std::optional<TRet> SEHGuarded(std::string_view name, TFunc&& func)
{
	__try
	{
		return func();
	}
	__except (1) // EXCEPTION_EXECUTE_HANDLER
	{
		// TODO: More info
		Log::Error(cs::Red(name), cs::Red(" SEH exception"));
		return std::nullopt;
	}
}

struct _unwrap_optional_tag {};

template <typename TFunc, typename TRet = typename std::invoke_result_t<TFunc>::value_type>
std::optional<TRet> SEHGuarded(_unwrap_optional_tag, std::string_view name, TFunc&& func)
{
	if (auto result = SEHGuarded<TFunc, TRet>(name, std::move(func)))
		return *result;

	return std::nullopt;
}
