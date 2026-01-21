#include "pch.hpp"

#include "common/SEHGuarded.hpp"

#include "common/Log.hpp"
#include "StackTracer/StackTracer.hpp"

#include "common/MyWindows.hpp"

int _FilterSEHException(std::string_view name, unsigned long code, struct _EXCEPTION_POINTERS* pointers)
{
	if (code != EXCEPTION_ACCESS_VIOLATION)
		return EXCEPTION_CONTINUE_SEARCH;

	Log::Error(cs::Red(name), cs::Red(" SEH exception"));

	// FIXME: never returns frames
	if (StackTracer::LogStackTrace(pointers) == 0)
		StackTracer::LogStackTrace();

	Log::Flush();

	return EXCEPTION_EXECUTE_HANDLER;
}
