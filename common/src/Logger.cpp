#include "pch.hpp"

#include "common/Logger.hpp"

#include <format>
#include <chrono>

Logger::~Logger()
{
}

void Logger::AddLine(Level level, Type type, std::initializer_list<cs::StyledString> line)
{
}

constexpr size_t TIME_STR_LEN = 23;

size_t Logger::GetSystemTimeStr(char* out, size_t out_size)
{
	if (!out)
		return TIME_STR_LEN;

	if (out_size < TIME_STR_LEN)
		return 0;

	auto now = std::chrono::system_clock::now();
	return std::format_to(out, "{:%F %T}", std::chrono::floor<std::chrono::milliseconds>(now)) - out;
}

std::string Logger::GetSystemTimeStr()
{
	std::string result(TIME_STR_LEN, '\0');
	GetSystemTimeStr(result.data(), result.size());
	return result;
}
