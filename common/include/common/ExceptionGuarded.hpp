#pragma once

#include "common/SEHGuarded.hpp"
#include "common/TryGuarded.hpp"

template <typename TFunc, typename TRet = std::invoke_result_t<TFunc>>
std::optional<TRet> ExceptionGuarded(std::string_view name, TFunc&& func)
{
	return SEHGuarded(_unwrap_optional_tag{}, name, [&]() {
		return TryGuarded(name, [&]() {
			return func();
		});
	});
}
