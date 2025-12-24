#pragma once

#include "common/Logger.hpp"


namespace Log
{


void Line(Logger::Level level, Logger::Type type, std::initializer_list<cs::ColoredString> _line);

inline void Line(Logger::Type type, std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Default, type, _line);
}

inline void Line(Logger::Level level, std::initializer_list<cs::ColoredString> _line)
{
	Line(level, Logger::Type::Default, _line);
}

inline void Line(std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Default, Logger::Type::Default, _line);
}

inline void Line(Logger::Level level, Logger::Type type, std::string_view _line)
{
	Line(level, type, { cs::normal(_line) });
}

inline void Line(Logger::Type type, std::string_view _line)
{
	Line(Logger::Level::Default, type, _line);
}

inline void Line(Logger::Level level, std::string_view _line)
{
	Line(level, Logger::Type::Default, _line);
}

inline void Line(std::string_view _line)
{
	Line(Logger::Level::Default, Logger::Type::Default, _line);
}



inline void Debug(Logger::Type type, std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Debug, type, _line);
}

inline void Debug(std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Debug, _line);
}

inline void Debug(Logger::Type type, std::string_view _line)
{
	Line(Logger::Level::Debug, type, _line);
}

inline void Debug(std::string_view _line)
{
	Line(Logger::Level::Debug, _line);
}



inline void Info(Logger::Type type, std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Info, type, _line);
}

inline void Info(std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Info, _line);
}

inline void Info(Logger::Type type, std::string_view _line)
{
	Line(Logger::Level::Info, type, _line);
}

inline void Info(std::string_view _line)
{
	Line(Logger::Level::Info, _line);
}



inline void Warn(Logger::Type type, std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Warning, type, _line);
}

inline void Warn(std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Warning, _line);
}

inline void Warn(Logger::Type type, std::string_view _line)
{
	Line(Logger::Level::Warning, type, _line);
}

inline void Warn(std::string_view _line)
{
	Line(Logger::Level::Warning, _line);
}



inline void Error(Logger::Type type, std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Error, type, _line);
}

inline void Error(std::initializer_list<cs::ColoredString> _line)
{
	Line(Logger::Level::Error, _line);
}

inline void Error(Logger::Type type, std::string_view _line)
{
	Line(Logger::Level::Error, type, _line);
}

inline void Error(std::string_view _line)
{
	Line(Logger::Level::Error, _line);
}


} // namespace Log
