#include "pch.hpp"

#include "StackTracer/StackTracer.hpp"

#include "common/Log.hpp"
#include "common/StringUtils.hpp"

#include <sstream>
#include <optional>
#include <dbghelp.h>

#pragma comment(lib, "dbghelp.lib")

struct Initializer
{
	std::optional<bool> initialized;
	HANDLE for_process = nullptr;

	~Initializer()
	{
		CleanupSymbols();
	}

	bool InitSymbols(HANDLE process)
	{
		if (!initialized.has_value())
		{
			SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

			if (SymInitialize(process, nullptr, TRUE))
			{
				initialized = true;
				for_process = process;
			}
			else
			{
				Log::Error("SymInitialize failed: " + su::u8(GetLastError()));
				initialized = false;
			}
		}

		return *initialized;
	}

	void CleanupSymbols()
	{
		if (for_process)
		{
			initialized = std::nullopt;
			SymCleanup(for_process);
		}
	}
};

static Initializer g_symbols_initializer;


static std::vector<StackTracer::StackFrame> WalkStack(HANDLE process, HANDLE thread)
{
	std::vector<StackTracer::StackFrame> frames;

	STACKFRAME64 stack_frame = {};
	CONTEXT context = {};

	context.ContextFlags = CONTEXT_FULL;
	RtlCaptureContext(&context);

	// Initialize stack frame based on architecture
#ifdef _M_IX86
	DWORD machine_type = IMAGE_FILE_MACHINE_I386;
	stack_frame.AddrPC.Offset = context.Eip;
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Offset = context.Ebp;
	stack_frame.AddrFrame.Mode = AddrModeFlat;
	stack_frame.AddrStack.Offset = context.Esp;
	stack_frame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	DWORD machine_type = IMAGE_FILE_MACHINE_AMD64;
	stack_frame.AddrPC.Offset = context.Rip;
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Offset = context.Rbp;
	stack_frame.AddrFrame.Mode = AddrModeFlat;
	stack_frame.AddrStack.Offset = context.Rsp;
	stack_frame.AddrStack.Mode = AddrModeFlat;
#else
	#error Unsupported architecture
#endif

	for (int frame_num = 0; frame_num < 128; frame_num++)
	{
		if (!StackWalk64(
			machine_type,
			process,
			thread,
			&stack_frame,
			&context,
			nullptr,
			SymFunctionTableAccess64,
			SymGetModuleBase64,
			nullptr))
		{
			break;
		}

		if (stack_frame.AddrPC.Offset == 0)
		{
			break;
		}

		StackTracer::StackFrame frame{};
		frame.address = stack_frame.AddrPC.Offset;

		// Try to get symbol name
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		DWORD64 displacement = 0;
		if (SymFromAddr(process, stack_frame.AddrPC.Offset, &displacement, symbol))
		{
			frame.name = symbol->Name;
		}
		else
		{
			frame.name = "<unknown>";
		}

		IMAGEHLP_LINE64 line = {};
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		DWORD line_displacement = 0;

		if (SymGetLineFromAddr64(process, stack_frame.AddrPC.Offset, &line_displacement, &line))
		{
			frame.line_number = line.LineNumber;
			frame.file_name = line.FileName;
		}

		// Get module name
		IMAGEHLP_MODULE64 module_info = {};
		module_info.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
		if (SymGetModuleInfo64(process, stack_frame.AddrPC.Offset, &module_info))
		{
			frame.module = module_info.ModuleName;
		}

		frames.push_back(frame);
	}

	return frames;
}

std::string StackTracer::GetStackTrace()
{
	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	std::stringstream ss;

	if (g_symbols_initializer.InitSymbols(process))
	{
		auto frames = WalkStack(process, thread);

		ss << "Stack Trace (" << frames.size() << " frames):\n";
		for (size_t i = 0; i < frames.size(); ++i)
		{
			const auto& frame = frames[i];
			ss << i << ": " << frame.name
				<< " [0x" << std::hex << frame.address << std::dec << "]";

			if (!frame.file_name.empty())
			{
				ss << " at " << frame.file_name << ":" << frame.line_number;
			}

			if (!frame.module.empty())
			{
				ss << " (" << frame.module << ")";
			}

			if (i != frames.size() - 1)
				ss << "\n";
		}
	}

	return std::move(ss).str();
}
