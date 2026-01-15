#include "pch.hpp"

#include "common/LoggerManager.hpp"
#include "common/Logger.hpp"

#include "common/ConsoleLogger.hpp"

static_assert(cs::_COLORS_VERSION == 1, "StyledString version doesn't match");

static constexpr std::string_view ANSII_COLORS[] {
	"\x1b[0m",
	"\x1b[97m",
	"\x1b[30m",
	"\x1b[90m",
	"\x1b[91m",
	"\x1b[92m",
	"\x1b[94m",
	"\x1b[93m",
	"\x1b[38;5;214m",
	"\x1b[95m",
	"\x1b[96m",
	"\x1b[35m",
	"\x1b[95m",
};

class ConsoleLogger : public Logger
{
public:

	ConsoleLogger()
		: logger_("Example Client")
	{
		cache_line_.reserve(4 * 1024);
	}

	// NOTE: ignores type
	virtual void AddLine(Level level, Type type, std::initializer_list<cs::StyledString> line)
	{
		if (level < min_level_)
			return;

		cache_line_.clear();

		cs::Color prev_color = (cs::Color)-1;
		auto add_color = [&](cs::Color color) {
			if (prev_color == color)
				return;

			const auto& color_str = ANSII_COLORS[(int)color];
			cache_line_.insert(cache_line_.end(), color_str.begin(), color_str.end());

			prev_color = color;
		};

		{
			char buf[32];
			assert(GetSystemTimeStr(nullptr, 0) <= sizeof(buf));
			size_t time_len = GetSystemTimeStr(buf, sizeof(buf));

			add_color(cs::Color::Gray);
			cache_line_.push_back('[');
			cache_line_.insert(cache_line_.end(), buf, buf + time_len);
			cache_line_.push_back(']');
			cache_line_.push_back(' ');
		}

		switch (level)
		{
			case Level::Debug:
				add_color(cs::Color::Gray);
				cache_line_.push_back('D');
				break;
			case Level::Info:
				add_color(cs::Color::Green);
				cache_line_.push_back('I');
				break;
			case Level::Warning:
				add_color(cs::Color::Yellow);
				cache_line_.push_back('W');
				break;
			case Level::Error:
				add_color(cs::Color::Red);
				cache_line_.push_back('E');
				break;
		}

		cache_line_.push_back(' ');

		for (const auto& part : line)
		{
			add_color(part.color);
			cache_line_.insert(cache_line_.end(), part.value.begin(), part.value.end());
		}

		cache_line_.push_back('\n');

		logger_.Add(std::string_view(cache_line_.data(), cache_line_.size()));
	}

private:

	Level min_level_ = Logger::Level::Debug;
	std::vector<char> cache_line_;

	common_loggers::ConsoleLogger logger_;
};

static RegisterLoggerStatic<ConsoleLogger> registered;
