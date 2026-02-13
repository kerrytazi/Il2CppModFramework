#include "pch.hpp"

#include "StackTracer/StackTracer.hpp"

#include "common/Log.hpp"

#include "common/MyWindows.hpp"

#include <sstream>
#include <optional>
#include <mutex>

struct StackFrame
{
	DWORD64 address;
	DWORD64 rva;
	std::string name;
	std::string module;
	DWORD line_number;
	std::string file_name;
};

class IStackTraceParser
{
public:

	virtual ~IStackTraceParser() {}
	virtual StackFrame ParseStackFrame(const STACKFRAME64& stack_frame) = 0;
};

static STACKFRAME64 InitStackFrame(const CONTEXT* context)
{
#if _M_X64
	STACKFRAME64 stack_frame{};
	stack_frame.AddrPC.Offset = context->Rip;
	stack_frame.AddrPC.Mode = AddrModeFlat;
	stack_frame.AddrFrame.Offset = context->Rbp;
	stack_frame.AddrFrame.Mode = AddrModeFlat;
	stack_frame.AddrStack.Offset = context->Rsp;
	stack_frame.AddrStack.Mode = AddrModeFlat;

	// StackWalk64 Can't traverse call stack if last call jumped on null.
	// Manually traversing one step back to the place where null was called.
	if (stack_frame.AddrPC.Offset == 0)
	{
		stack_frame.AddrPC.Offset = *(DWORD64*)stack_frame.AddrStack.Offset;
		stack_frame.AddrStack.Offset -= 8;
	}
#else
	#error Unsupported architecture
#endif

	return stack_frame;
}

#if defined(ICMF_STACK_TRACER_DIA)
#include <dia2.h>
#pragma comment(lib, "diaguids.lib")
//#pragma comment(lib, "msdia140.lib")
#endif // defined(ICMF_STACK_TRACER_DIA)

#if defined(ICMF_STACK_TRACER_DBGHELP)
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

class StackTraceParserDbgHelp final : public IStackTraceParser
{
public:

	StackTraceParserDbgHelp()
	{
		process_ = GetCurrentProcess();

		std::unique_lock lock(mtx_);

		SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

		if (!SymInitialize(process_, nullptr, TRUE))
			Log::Error("StackTracerDbgHelp SymInitialize failed: ", GetLastError());
	}

	virtual ~StackTraceParserDbgHelp() override
	{
		std::unique_lock lock(mtx_);
		SymCleanup(process_);
	}

	virtual StackFrame ParseStackFrame(const STACKFRAME64& stack_frame) override
	{
		std::unique_lock lock(mtx_);

		StackFrame frame{};
		frame.address = stack_frame.AddrPC.Offset;

		SYMBOL_INFO_PACKAGE package;
		package.si.SizeOfStruct = sizeof(package.si);
		package.si.MaxNameLen = MAX_SYM_NAME;

		DWORD64 displacement = 0;
		if (SymFromAddr(process_, stack_frame.AddrPC.Offset, &displacement, &package.si))
			frame.name = std::string(package.si.Name, package.si.NameLen);
		else
			frame.name = "<unknown>";

		IMAGEHLP_LINE64 line = {};
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		DWORD line_displacement = 0;

		if (SymGetLineFromAddr64(process_, stack_frame.AddrPC.Offset, &line_displacement, &line))
		{
			frame.line_number = line.LineNumber;
			frame.file_name = line.FileName;
		}

		// Get module name
		IMAGEHLP_MODULE64 module_info = {};
		module_info.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
		if (SymGetModuleInfo64(process_, stack_frame.AddrPC.Offset, &module_info))
		{
			frame.module = module_info.ModuleName;
			frame.rva = stack_frame.AddrPC.Offset - module_info.BaseOfImage;
		}

		return frame;
	}

private:

	HANDLE process_ = nullptr;
	std::mutex mtx_;
};

#endif // defined(ICMF_STACK_TRACER_DBGHELP)

static std::unique_ptr<IStackTraceParser> g_stack_tracer;
static IStackTraceParser* GetStackTraceParser()
{
	if (!g_stack_tracer)
#if defined(ICMF_STACK_TRACER_DIA)
		g_stack_tracer = std::make_unique<StackTraceParserDIA>();
#elif defined(ICMF_STACK_TRACER_DBGHELP)
		g_stack_tracer = std::make_unique<StackTraceParserDbgHelp>();
#elif defined(ICMF_STACK_TRACER_NONE)
		g_stack_tracer = std::make_unique<StackTraceParserNone>();
#endif

	return g_stack_tracer.get();
}

void _StackTracerCleanup()
{
	g_stack_tracer.reset();
}

static std::vector<StackFrame> WalkStack(const CONTEXT* context)
{
	std::vector<StackFrame> frames;

	HANDLE process = GetCurrentProcess();
	HANDLE thread = GetCurrentThread();

	auto stack_frame = InitStackFrame(context);

	for (int frame_num = 0; frame_num < 128; frame_num++)
	{
		if (!StackWalk64(
			IMAGE_FILE_MACHINE_AMD64,
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

		frames.push_back(GetStackTraceParser()->ParseStackFrame(stack_frame));
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
			cs::Green(Log::Pad(i, ' ', 2)), cs::Red(": "), cs::Red("["), cs::Cyan("0x"), cs::Cyan((void*)frame.address), cs::Red("]"),
			cs::Red(m ? " (" : ""), cs::Yellow(m ? frame.module : ""), cs::Red(m ? " + " : ""), cs::Yellow(m ? su::u8(frame.rva, 16) : ""), cs::Red(m ? ")" : ""),
			" ", cs::Red(frame.name),
			cs::Red(n ? " at " : ""), cs::Yellow(n ? frame.file_name : ""), cs::Red(n ? ":" : ""), cs::Red(n ? su::u8(frame.line_number) : "")
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
		ss << i << ": " << " [0x" << std::hex << frame.address << std::dec << "]";

		if (!frame.module.empty())
		{
			ss << " (" << frame.module << " + " << std::hex << frame.rva << std::dec << ")";
		}

		ss << " " << frame.name;

		if (!frame.file_name.empty())
		{
			ss << " at " << frame.file_name << ":" << frame.line_number;
		}

		if (i != frames.size() - 1)
			ss << "\n";
	}

	return std::move(ss).str();
}

std::string StackTracer::GetStackTrace()
{
	CONTEXT context{};
	context.ContextFlags = CONTEXT_FULL;
	RtlCaptureContext(&context);

	auto frames = WalkStack(&context);
	return FormatFrames(frames);
}

std::string StackTracer::GetStackTrace(_EXCEPTION_POINTERS* pointers)
{
	if (!pointers)
		return "StackTracer::GetStackTrace(pointers) failed. pointers == null";

	if (!pointers->ContextRecord)
		return "StackTracer::GetStackTrace(pointers) failed. pointers->ContextRecord == null";

	auto frames = WalkStack(pointers->ContextRecord);
	return FormatFrames(frames);
}

void StackTracer::LogStackTrace()
{
	CONTEXT context{};
	context.ContextFlags = CONTEXT_FULL;
	RtlCaptureContext(&context);

	auto frames = WalkStack(&context);
	LogFrames(frames);
}

int StackTracer::LogStackTrace(_EXCEPTION_POINTERS* pointers)
{
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

	CONTEXT context = *pointers->ContextRecord;
	auto frames = WalkStack(&context);

	if (frames.empty())
	{
		Log::Error(cs::Red("StackTracer::LogStackTrace(pointers) failed. Zero frames found"));
		return 0;
	}

	LogFrames(frames);
	return (int)frames.size();
}
