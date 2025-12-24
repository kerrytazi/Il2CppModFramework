#include "pch.hpp"

#include "common/Logger.hpp"

#include <format>
#include <chrono>

Logger::~Logger()
{
}

void Logger::AddLine(Level level, Type type, std::initializer_list<cs::ColoredString> line)
{
}

std::string Logger::GetSystemTimeStr()
{
	auto now = std::chrono::system_clock::now();
	return std::format("{:%F %T}", std::chrono::floor<std::chrono::milliseconds>(now));
}
