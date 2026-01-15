#include "pch.hpp"

#include "common/LoggerManager.hpp"

#include "common/Logger.hpp"
#include "common/StyledString.hpp"
#include "common/LazyStatic.hpp"

struct RegisteredLogger
{
	std::function<std::shared_ptr<Logger>()> create;
	int priority = 0;
};

static LazyStatic<std::vector<RegisteredLogger>> g_registered_loggers;

void _LoggerCleanup()
{
	g_registered_loggers = {};
}

void RegisterLogger(std::function<std::shared_ptr<Logger>()> create_logger, int priority /*= 50*/)
{
	const auto proj = [](const RegisteredLogger& rlogger) {
		return rlogger.priority;
	};

	auto it = std::ranges::lower_bound(*g_registered_loggers, priority, std::less<>{}, proj);

	g_registered_loggers->insert(it, RegisteredLogger{
		.create = std::move(create_logger),
		.priority = priority,
	});
}

LoggerManager::LoggerManager()
{
	loggers_.reserve(g_registered_loggers->size());

	for (const auto& rlogger : *g_registered_loggers)
		loggers_.push_back(rlogger.create());
}

LoggerManager::~LoggerManager()
{
}

void LoggerManager::LogLine(Logger::Level level, Logger::Type type, std::initializer_list<cs::StyledString> line)
{
	std::unique_lock lock(mtx_);

	for (const auto& logger : loggers_)
		logger->AddLine(level, type, line);
}
