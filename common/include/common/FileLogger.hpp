#pragma once

#include <string>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <memory>

namespace common_loggers
{

class FileLogger
{
public:

	struct LoggerFlags
	{
		bool append = false;
		bool flush_each_line = false;
		size_t buffer_size = 1 * 1024 * 1024;

		static LoggerFlags Default() { return {}; }
		static LoggerFlags WithFlushEachLine() { return { .flush_each_line = true }; }
	};

	FileLogger(std::string filename, const LoggerFlags& logger_flags = LoggerFlags::Default())
		: flags_{ logger_flags }
	{
		auto flags = std::ios_base::out;

		if (flags_.append)
			flags |= std::ios_base::ate;

		std::filesystem::create_directories(std::filesystem::path(filename).parent_path());

		buffer_ = std::make_unique<char[]>(flags_.buffer_size);
		file_.rdbuf()->pubsetbuf(buffer_.get(), flags_.buffer_size);

		file_.open(filename, flags);

		if (file_.bad())
			throw std::runtime_error("Can't open log file for writing: " + filename);
	}

	void Add(std::string_view msg)
	{
		if (msg.empty())
			return;

		file_ << msg;

		if (flags_.flush_each_line && msg.back() == '\n')
			file_.flush();
	}

	void AddLine(std::string_view line)
	{
		if (!line.empty())
			file_ << line;

		file_ << "\n";

		if (flags_.flush_each_line)
			file_.flush();
	}

private:

	std::ofstream file_;
	std::unique_ptr<char[]> buffer_;

	LoggerFlags flags_;
};

} // namespace common_loggers
