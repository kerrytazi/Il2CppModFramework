#pragma once

#include "common/Logger.hpp"

#include <vector>
#include <memory>
#include <functional>

void RegisterLogger(std::function<std::shared_ptr<Logger>()> create_logger, int priority = 50);

template <typename T, int priority = 50>
struct RegisterLoggerStatic
{
	inline static bool registered = ([](){
		RegisterLogger([]() -> std::shared_ptr<Logger> {
			return std::make_unique<T>();
		}, priority);

		return true;
	})();
};

class LoggerManager
{
public:

	LoggerManager();
	~LoggerManager();

	void LogLine(Logger::Level level, Logger::Type type, std::initializer_list<cs::ColoredString> line);

private:

	std::vector<std::shared_ptr<Logger>> loggers_;
};

inline LoggerManager* g_logger_manager = nullptr;
