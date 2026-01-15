#pragma once

#include "System/Exception.hpp"

#include "common/Log.hpp"

#include <string_view>
#include <type_traits>

template <typename TFunc, typename TRet = std::invoke_result_t<TFunc>>
std::optional<TRet> TryGuarded(std::string_view name, TFunc&& func)
{
	try
	{
		func();
	}
	catch (int ex)
	{
		Log::Error(cs::Red(name), cs::Red(" catch int: "), cs::Red(ex));
	}
	catch (const std::exception& ex)
	{
		Log::Error(cs::Red(name), cs::Red(" catch std::exception: "), cs::Red(ex.what()));
	}
	catch (Il2CppExceptionWrapper& wrapper)
	{
		Log::Error(cs::Red(name), cs::Red(" catch Il2CppExceptionWrapper message: "), cs::Red(wrapper.ex->GetMessage()->AsU16StringView()));
		Log::Error(cs::Red(name), cs::Red(" catch Il2CppExceptionWrapper source: "), cs::Red(wrapper.ex->GetSource()->AsU16StringView()));
		Log::Error(cs::Red(name), cs::Red(" catch Il2CppExceptionWrapper stack: "), cs::Red(wrapper.ex->GetStackTrace()->AsU16StringView()));
	}
	catch (...)
	{
		Log::Error(cs::Red(name), cs::Red(" Exception ?"));
	}

	return std::nullopt;
}
