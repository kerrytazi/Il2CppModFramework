#include "pch.hpp"

#include "common/TryGuarded.hpp"

#include "common/Log.hpp"

#include "System/String.hpp"

#include "StackTracer/StackTracer.hpp"

void _HandleTryException(std::string_view name, int ex)
{
	Log::Error(cs::Red(name), cs::Red(" catch int: "), cs::Red(ex));
	StackTracer::LogStackTrace();
	Log::Flush();
}

void _HandleTryException(std::string_view name, const std::exception& ex)
{
	Log::Error(cs::Red(name), cs::Red(" catch std::exception: "), cs::Red(ex.what()));
	StackTracer::LogStackTrace();
	Log::Flush();
}

void _HandleTryException(std::string_view name, const Il2CppExceptionWrapper& wrapper)
{
	Log::Error(cs::Red(name), cs::Red(" catch Il2CppExceptionWrapper message: "), cs::Red(wrapper.ex->GetMessage()->AsU16StringView()));
	Log::Error(cs::Red(name), cs::Red(" catch Il2CppExceptionWrapper source: "), cs::Red(wrapper.ex->GetSource()->AsU16StringView()));
	Log::Error(cs::Red(name), cs::Red(" catch Il2CppExceptionWrapper stack: "), cs::Red(wrapper.ex->GetStackTrace()->AsU16StringView()));
	StackTracer::LogStackTrace();
	Log::Flush();
}

void _HandleTryException(std::string_view name)
{
	Log::Error(cs::Red(name), cs::Red(" Exception ?"));
	StackTracer::LogStackTrace();
	Log::Flush();
}
