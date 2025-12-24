#include "pch.hpp"

#include "common/Log.hpp"
#include "common/LoggerManager.hpp"


void Log::Line(Logger::Level level, Logger::Type type, std::initializer_list<cs::ColoredString> _line)
{
	if (!g_logger_manager)
		return;

	g_logger_manager->LogLine(level, type, _line);
}
