#pragma once

#include "common/StyledString.hpp"

#include "common/MyWindows.hpp"

#include <string>
#include <string_view>
#include <atomic>

inline std::atomic<int> g_num_consoles = 0;

inline void SetupConsole(const std::string& console_name)
{
	if (g_num_consoles.fetch_add(1) == 0)
	{
		AllocConsole();

		SetConsoleOutputCP(CP_UTF8);
		SetConsoleCP(CP_UTF8);

		for (DWORD handle_tag : { STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE })
		{
			HANDLE handle = GetStdHandle(handle_tag);

			DWORD orig_mode = 0;
			GetConsoleMode(handle, &orig_mode);
			SetConsoleMode(handle, orig_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
		}

		SetConsoleTitleA(console_name.c_str());

		FILE* fp;
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		freopen_s(&fp, "CONIN$", "r", stdin);
	}
}

inline void CleanupConsole()
{
	if (g_num_consoles.fetch_sub(1) == 1)
	{
		fclose(stdout);
		fclose(stderr);
		fclose(stdin);

		FreeConsole();
	}
}

namespace common_loggers
{

class ConsoleLogger
{
public:

	ConsoleLogger(const std::string& console_name)
	{
		SetupConsole(console_name);
		hconsole = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	~ConsoleLogger()
	{
		CleanupConsole();
	}

	void Add(std::string_view msg)
	{
		DWORD chars_written = 0;
		WriteConsoleA(hconsole, msg.data(), (DWORD)msg.size(), &chars_written, NULL);
	}

	void AddLine(std::string_view line)
	{
		Add(line);

		DWORD chars_written = 0;
		WriteConsoleA(hconsole, "\n", 1, &chars_written, NULL);
	}

private:

	HANDLE hconsole = nullptr;
};

} // namespace common_loggers
