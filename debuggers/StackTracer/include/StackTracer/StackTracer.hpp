#pragma once

#include "common/MyWindows.hpp"

#include <vector>
#include <string>

namespace StackTracer
{

struct StackFrame
{
	DWORD64 address;
	std::string name;
	std::string module;
	DWORD line_number;
	std::string file_name;
};

std::string GetStackTrace();

} // namespace StackTracer
