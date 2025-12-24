#include "pch.hpp"

#include "common/FileLogger.hpp"
#include "common/LoggerManager.hpp"
#include "common/Logger.hpp"

class FileLogger : public Logger
{
public:

	FileLogger()
		: debug_("client/debug.log")
		, info_("client/info.log")
		, warn_("client/warn.log")
		, error_("client/error.log")
		, full_("client/full.log")
	{
	}

	// NOTE: ignores colors
	// NOTE: ignores type
	virtual void AddLine(Level level, Type type, std::initializer_list<cs::ColoredString> line)
	{
		assert(line.size() > 0);

		std::stringstream ss;

		ss << "[" << GetSystemTimeStr() << "] ";

		switch (level)
		{
			case Level::Debug:   ss << "D "; break;
			case Level::Info:    ss << "I "; break;
			case Level::Warning: ss << "W "; break;
			case Level::Error:   ss << "E "; break;
		}

		for (const auto& cstr : line)
			ss << cstr.str;

		std::string full_line = std::move(ss).str();

		switch (level)
		{
			case Level::Debug:   if (min_level_ >= level) debug_.AddLine(full_line); break;
			case Level::Info:    if (min_level_ >= level) info_.AddLine(full_line); break;
			case Level::Warning: if (min_level_ >= level) warn_.AddLine(full_line); break;
			case Level::Error:   if (min_level_ >= level) error_.AddLine(full_line); break;
		}

		full_.AddLine(full_line);
	}

private:

	Level min_level_ = Logger::Level::Debug;

	common_modules::FileLogger debug_;
	common_modules::FileLogger info_;
	common_modules::FileLogger warn_;
	common_modules::FileLogger error_;
	common_modules::FileLogger full_;
};

static RegisterLoggerStatic<FileLogger> registered;
