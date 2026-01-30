#include "pch.hpp"

#include "StackTracer/StackTracer.hpp"

#include "common/Log.hpp"

#include "common/MyWindows.hpp"

#include <sstream>
#include <optional>
#include <mutex>

#if defined(UC_STACK_TRACER_DBGHELP)
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
				Log::Error("SymInitialize failed: ", GetLastError());
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
static std::mutex g_mtx;

#elif defined(UC_STACK_TRACER_DIA)
#include <dia2.h>
#pragma comment(lib, "diaguids.lib")
#elif !defined(UC_STACK_TRACER_NONE)
#error UC_STACK_TRACER Implementation not defined
#endif

#if !defined(UC_STACK_TRACER_NONE)
struct StackFrame
{
	DWORD64 address;
	std::string name;
	std::string module;
	DWORD line_number;
	std::string file_name;
};

static std::vector<StackFrame> WalkStack(HANDLE process, HANDLE thread, const CONTEXT* context)
{
	std::vector<StackFrame> frames;

	// Initialize stack frame based on architecture
#ifdef _M_IX86
	STACKFRAME stack_frame{};
	DWORD machine_type = IMAGE_FILE_MACHINE_I386;
	stack_frame.AddrPC.Offset = context->Eip;
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Offset = context->Ebp;
	stack_frame.AddrFrame.Mode = AddrModeFlat;
	stack_frame.AddrStack.Offset = context->Esp;
	stack_frame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	STACKFRAME64 stack_frame{};
	DWORD machine_type = IMAGE_FILE_MACHINE_AMD64;
	stack_frame.AddrPC.Offset = context->Rip;
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Offset = context->Rbp;
	stack_frame.AddrFrame.Mode = AddrModeFlat;
	stack_frame.AddrStack.Offset = context->Rsp;
	stack_frame.AddrStack.Mode = AddrModeFlat;
#else
	#error Unsupported architecture
#endif

	// StackWalk64 Can't traverse call stack if last call jumped on null.
	// Manually traversing one step back to the place where null was called.
	if (stack_frame.AddrPC.Offset == 0)
	{
		stack_frame.AddrPC.Offset = *(DWORD64*)stack_frame.AddrStack.Offset;
		stack_frame.AddrStack.Offset -= 8;
	}

	for (int frame_num = 0; frame_num < 128; frame_num++)
	{
		if (!StackWalk64(
			machine_type,
			process,
			thread,
			&stack_frame,
			(void*)context,
			nullptr,
			&SymFunctionTableAccess64,
			&SymGetModuleBase64,
			nullptr))
		{
			break;
		}

		if (stack_frame.AddrPC.Offset == 0)
			break;

		StackFrame frame{};
		frame.address = stack_frame.AddrPC.Offset;

		SYMBOL_INFO_PACKAGE package;
		package.si.SizeOfStruct = sizeof(package.si);
		package.si.MaxNameLen = MAX_SYM_NAME;

		DWORD64 displacement = 0;
		if (SymFromAddr(process, stack_frame.AddrPC.Offset, &displacement, &package.si))
		{
			frame.name = std::string(package.si.Name, package.si.NameLen);
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

static void LogFrames(const std::vector<StackFrame>& frames)
{
	Log::Error(cs::Red("Stack Trace ("), cs::Red(frames.size()), cs::Red(" frames):"));
	for (size_t i = 0; i < frames.size(); ++i)
	{
		const auto& frame = frames[i];
		bool n = !frame.file_name.empty();
		bool m = !frame.module.empty();

		Log::Error(
			cs::Green(Log::Pad(i, ' ', 2)), cs::Red(": "), cs::Red(frame.name), cs::Red(" ["), cs::Cyan("0x"), cs::Cyan((void*)frame.address), cs::Red("]"),
			cs::Red(n ? " at " : ""), cs::Yellow(n ? frame.file_name : ""), cs::Red(n ? ":" : ""), cs::Red(n ? su::u8(frame.line_number) : ""),
			cs::Red(m ? " (" : ""), cs::Yellow(m ? frame.module : ""), cs::Red(m ? ")" : "")
		);
	}
}

static std::string FormatFrames(const std::vector<StackFrame>& frames)
{
	std::stringstream ss;

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

	return std::move(ss).str();
}
#endif // !defined(UC_STACK_TRACER_NONE)

std::string StackTracer::GetStackTrace()
{
#if defined(UC_STACK_TRACER_NONE)
	return "<disabled>";
#else
	std::unique_lock lock(g_mtx);

	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	if (!g_symbols_initializer.InitSymbols(process))
		return "StackTracer::GetStackTrace() failed. Can't InitSymbols";

	CONTEXT context{};
	context.ContextFlags = CONTEXT_FULL;
	RtlCaptureContext(&context);

	auto frames = WalkStack(process, thread, &context);
	return FormatFrames(frames);
#endif // defined(UC_STACK_TRACER_NONE)
}

std::string StackTracer::GetStackTrace(_EXCEPTION_POINTERS* pointers)
{
#if defined(UC_STACK_TRACER_NONE)
	return "<disabled>";
#else
	std::unique_lock lock(g_mtx);

	if (!pointers)
		return "StackTracer::GetStackTrace(pointers) failed. pointers == null";

	if (!pointers->ContextRecord)
		return "StackTracer::GetStackTrace(pointers) failed. pointers->ContextRecord == null";

	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	if (g_symbols_initializer.InitSymbols(process))
		return "StackTracer::GetStackTrace(pointers) failed. Can't InitSymbols";

	auto frames = WalkStack(process, thread, pointers->ContextRecord);
	return FormatFrames(frames);
#endif // defined(UC_STACK_TRACER_NONE)
}

void StackTracer::LogStackTrace()
{
#if !defined(UC_STACK_TRACER_NONE)
	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	std::unique_lock lock(g_mtx);

	if (g_symbols_initializer.InitSymbols(process))
		Log::Error(cs::Red("StackTracer::LogStackTrace() failed. Can't InitSymbols"));

	CONTEXT context{};
	context.ContextFlags = CONTEXT_FULL;
	RtlCaptureContext(&context);

	auto frames = WalkStack(process, thread, &context);
	LogFrames(frames);
#endif // !defined(UC_STACK_TRACER_NONE)
}

int StackTracer::LogStackTrace(_EXCEPTION_POINTERS* pointers)
{
#if defined(UC_STACK_TRACER_NONE)
	return 0;
#else
	if (!pointers)
	{
		Log::Error(cs::Red("StackTracer::LogStackTrace(pointers) failed. pointers == null"));
		return 0;
	}

	if (!pointers->ContextRecord)
	{
		Log::Error(cs::Red("StackTracer::LogStackTrace(pointers) failed. pointers->ContextRecord == null"));
		return 0;
	}

	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	std::unique_lock lock(g_mtx);

	if (!g_symbols_initializer.InitSymbols(process))
	{
		Log::Error(cs::Red("StackTracer::LogStackTrace(pointers) failed. Can't InitSymbols"));
		return 0;
	}

	CONTEXT context = *pointers->ContextRecord;
	auto frames = WalkStack(process, thread, &context);

	if (frames.empty())
	{
		Log::Error(cs::Red("StackTracer::LogStackTrace(pointers) failed. Zero frames found"));
		return 0;
	}

	LogFrames(frames);
	return (int)frames.size();
#endif // defined(UC_STACK_TRACER_NONE)
}
