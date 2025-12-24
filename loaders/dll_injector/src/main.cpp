
#include <Windows.h>
#include <tlhelp32.h>

#include <cstdio>
#include <cstring>
#include <utility>
#include <vector>
#include <string>

template<typename TLambda>
class COnScopeExit
{

	TLambda m_Lambda;

	COnScopeExit(const COnScopeExit &) = delete;
	COnScopeExit &operator = (const COnScopeExit &) = delete;
	COnScopeExit(COnScopeExit &&) = delete;
	COnScopeExit &operator = (COnScopeExit &&) = delete;

public:

	COnScopeExit(TLambda &&Lambda) :
		m_Lambda(std::move(Lambda))
	{
	}

	~COnScopeExit()
	{
		m_Lambda();
	}
};

struct CProgramArgs
{
	const char *TargetProcName = nullptr;
	const char *DllName = nullptr;
	int TargetProcID = -1;
	bool bDebugInfo = false;
};

static CProgramArgs ParseArgs(int Argc, char **Argv)
{
	CProgramArgs Result;

	// Remove current executable name
	int Index = 1;

	// We need at least two arguments: tag and value
	while (Index < Argc)
	{
		if (::strcmp(Argv[Index], "-t") == 0 || ::strcmp(Argv[Index], "--target-name") == 0)
		{
			if (Index + 1 < Argc)
			{
				Result.TargetProcName = Argv[Index + 1];
				Index += 2;
			}
			else
			{
				::fprintf(stderr, "Warn: Expected process name after %s\n", Argv[Index]);
				++Index;
			}
		}
		else
		if (::strcmp(Argv[Index], "-i") == 0 || ::strcmp(Argv[Index], "--target-id") == 0)
		{
			if (Index + 1 < Argc)
			{
				int TargetID = -1;

				if (sscanf_s(Argv[Index + 1], "%i", &TargetID))
				{
					Result.TargetProcID = TargetID;
				}
				else
				{
					::fprintf(stderr, "Warn: Can't parse target process id = '%s'\n", Argv[Index + 1]);
				}

				Index += 2;
			}
			else
			{
				::fprintf(stderr, "Warn: Expected process id after %s\n", Argv[Index]);
				++Index;
			}
		}
		else
		if (::strcmp(Argv[Index], "-d") == 0 || ::strcmp(Argv[Index], "--dll-name") == 0)
		{
			if (Index + 1 < Argc)
			{
				Result.DllName = Argv[Index + 1];
				Index += 2;
			}
			else
			{
				::fprintf(stderr, "Warn: Expected dll file name after %s\n", Argv[Index]);
				++Index;
			}
		}
		else
		if (::strcmp(Argv[Index], "-g") == 0 || ::strcmp(Argv[Index], "--debug") == 0)
		{
			Result.bDebugInfo = true;
			++Index;
		}
		else
		{
			::fprintf(stderr, "Warn: Unknown parameter = '%s'\n", Argv[Index]);
			++Index;
		}
	}

	return Result;
}

int main(int Argc, char **Argv)
{
	CProgramArgs Args = ::ParseArgs(Argc, Argv);

	if ((Args.TargetProcID == -1 && Args.TargetProcName == nullptr) || Args.DllName == nullptr)
	{
		::puts("");
		::puts("Usage:");
		::puts("    -t|--target-name <name>: process name or part of name");
		::puts("    -i|--target-id <id>: process id");
		::puts("    -d|--dll-name <name>: dll file name");
		::puts("    -g|--debug: print more info");
		::puts("");
		::puts("    Example: injector.exe -t SomeProcess.exe -d MyHack.dll");
		::puts("");

		return EXIT_SUCCESS;
	}

	HANDLE ToolhelpHandle = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (!ToolhelpHandle || ToolhelpHandle == INVALID_HANDLE_VALUE)
	{
		::fprintf(stderr, "Error: Can't CreateToolhelp32Snapshot\n");
		return EXIT_FAILURE;
	}

	COnScopeExit CloseToolhelpHandle_([&]() { ::CloseHandle(ToolhelpHandle); ToolhelpHandle = INVALID_HANDLE_VALUE; });

	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!::Process32First(ToolhelpHandle, &ProcEntry))
	{
		::fprintf(stderr, "Error: Can't Process32First\n");
		return EXIT_FAILURE;
	}

	if (Args.bDebugInfo)
		::fprintf(stdout, "[debug] Process list begin\n");

	struct CProcEntryInfo
	{
		int ID = -1;
		std::string Name;
	};

	std::vector<CProcEntryInfo> FoundProcs;

	do
	{
		bool bFound = false;

		if (Args.TargetProcID != -1)
		{
			if (Args.TargetProcID == ProcEntry.th32ProcessID)
			{
				CProcEntryInfo Info;
				Info.ID = ProcEntry.th32ProcessID;
				Info.Name = ProcEntry.szExeFile;
				FoundProcs.push_back(std::move(Info));

				bFound = true;
			}
		}

		if (Args.TargetProcName)
		{
			if (::strstr(ProcEntry.szExeFile, Args.TargetProcName))
			{
				CProcEntryInfo Info;
				Info.ID = ProcEntry.th32ProcessID;
				Info.Name = ProcEntry.szExeFile;
				FoundProcs.push_back(std::move(Info));

				bFound = true;
			}
		}

		if (Args.bDebugInfo)
			::fprintf(stdout, "[debug]   %c Process[%lu] %s\n", bFound ? '+' : '-', ProcEntry.th32ProcessID, ProcEntry.szExeFile);

	} while (::Process32Next(ToolhelpHandle, &ProcEntry));

	if (Args.bDebugInfo)
		::fprintf(stdout, "[debug] Process list end\n");

	CProcEntryInfo *pInfo;

	if (FoundProcs.size() == 0)
	{
		::fprintf(stderr, "Error: Can't find any matching process\n");
		return EXIT_FAILURE;
	}
	else
	if (FoundProcs.size() >= 2)
	{
		::fprintf(stdout, "Choose process:\n");

		int ProcsCount = static_cast<int>(FoundProcs.size());

		for (int i = 0; i < ProcsCount; ++i)
			::fprintf(stdout, " %2d Process[%i] %s\n", i, FoundProcs[i].ID, FoundProcs[i].Name.c_str());

		int ChoosedIndex;

		while (::scanf_s("%i", &ChoosedIndex) != 1 && ChoosedIndex >= 0 && ChoosedIndex < ProcsCount);

		pInfo = &FoundProcs[ChoosedIndex];
	}
	else
	{
		pInfo = &FoundProcs[0];
	}

	if (Args.bDebugInfo)
		::fprintf(stdout, "[debug] Attach to %d %s\n", pInfo->ID, pInfo->Name.c_str());

	if (Args.bDebugInfo)
		::fprintf(stdout, "[debug] Trying to open process with ALL_ACCESS. PID = %d\n", pInfo->ID);

	HANDLE TargetProcHandler = ::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, 0, pInfo->ID);

	if (!TargetProcHandler || TargetProcHandler == INVALID_HANDLE_VALUE)
	{
		::fprintf(stderr, "Error: Can't OpenProcess\n");
		return EXIT_FAILURE;
	}

	COnScopeExit CloseTargetProcHandle_([&]() { ::CloseHandle(TargetProcHandler); TargetProcHandler = INVALID_HANDLE_VALUE; });

	{
		if (Args.bDebugInfo)
			::fprintf(stdout, "[debug] Trying to allocate RW memory in process. PID = %d, size = %d\n", pInfo->ID, (int)MAX_PATH);

		void *pTargetDllPathMemoryBlock = ::VirtualAllocEx(TargetProcHandler, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

		if (!pTargetDllPathMemoryBlock)
		{
			::fprintf(stderr, "Error: Can't VirtualAllocEx for target process\n");
			return EXIT_FAILURE;
		}

		if (Args.bDebugInfo)
			::fprintf(stdout, "[debug] Trying to write to process memory. pointer = %llu, data = '%s'\n", (unsigned long long)pTargetDllPathMemoryBlock, Args.DllName);

		::WriteProcessMemory(TargetProcHandler, pTargetDllPathMemoryBlock, Args.DllName, ::strlen(Args.DllName) + 1, 0);

		{
			if (Args.bDebugInfo)
				::fprintf(stdout, "[debug] Trying to create thread with a LoadLibrary routine\n");

			HANDLE LoadLibraryThread = ::CreateRemoteThread(TargetProcHandler, 0, 0, (LPTHREAD_START_ROUTINE)(LoadLibrary), pTargetDllPathMemoryBlock, 0, 0);

			if (!LoadLibraryThread || LoadLibraryThread == INVALID_HANDLE_VALUE)
			{
				::fprintf(stderr, "Error: Can't CreateRemoteThread\n");
				return EXIT_FAILURE;
			}

			::CloseHandle(LoadLibraryThread);
		}
	}

	if (Args.bDebugInfo)
		::fprintf(stdout, "[debug] Success\n");

	return EXIT_SUCCESS;
}
