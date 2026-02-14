#pragma once

#include "common/simple_generator.hpp"
#include "common/MyWindows.hpp"

#include <string_view>
#include <ranges>
#include <stdexcept>

namespace win32_utils
{

typedef struct _PEB_LDR_DATA
{
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE Reserved1[16];
	PVOID Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

typedef void (* PPS_POST_PROCESS_INIT_ROUTINE)();

typedef struct _PEB
{
	BYTE Reserved1[2];
	BYTE BeingDebugged;
	BYTE Reserved2[1];
	PVOID Reserved3[2];
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	PVOID Reserved4[3];
	PVOID AtlThunkSListPtr;
	PVOID Reserved5;
	ULONG Reserved6;
	PVOID Reserved7;
	ULONG Reserved8;
	ULONG AtlThunkSListPtr32;
	PVOID Reserved9[45];
	BYTE Reserved10[96];
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE Reserved11[128];
	PVOID Reserved12[1];
	ULONG SessionId;
} PEB, * PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	// ... other fields may exist depending on Windows version
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

inline PPEB_LDR_DATA GetPebLdrData()
{
#ifdef _WIN64
	PPEB pPeb = (PPEB)__readgsqword(0x60);
#else
	PPEB pPeb = (PPEB)__readfsdword(0x30);
#endif

	return pPeb->Ldr;
}

inline auto ModulesView()
{
	auto table_entry_to_entry = [](PLIST_ENTRY pListEntry) -> PLDR_DATA_TABLE_ENTRY {
		return CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
	};

	auto ldr = GetPebLdrData();

	auto advance = [ldr](PLIST_ENTRY& pListEntry, auto mark_done) {
		pListEntry = pListEntry->Flink;

		if (pListEntry == &ldr->InLoadOrderModuleList)
			mark_done();
	};

	return make_simple_generator<PLDR_DATA_TABLE_ENTRY, PLIST_ENTRY, decltype(table_entry_to_entry)>(PLIST_ENTRY(ldr->InLoadOrderModuleList.Flink), std::move(advance));
}

inline void* FindModule(std::wstring_view module_name)
{
	for (auto pEntry : ModulesView())
	{
		size_t dll_name_length = pEntry->BaseDllName.Length / sizeof(WCHAR);

		if (dll_name_length != module_name.size())
			continue;

		if (module_name == std::wstring_view(pEntry->BaseDllName.Buffer, dll_name_length))
			return pEntry->DllBase;
	}

	return nullptr;
}

struct ProcEntry
{
	std::string_view name;
	const void* func;
};

inline auto ProcsView(const void* module)
{
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)module;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		throw std::runtime_error("ProcsView: Invalid IMAGE_DOS_SIGNATURE");

	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((BYTE*)module + pDosHeader->e_lfanew);
	if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE)
		throw std::runtime_error("ProcsView: Invalid IMAGE_NT_SIGNATURE");

	IMAGE_OPTIONAL_HEADER optionalHeader = pNtHeaders->OptionalHeader;
	PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)module +
		optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	const DWORD* pFunctions = (DWORD*)((BYTE*)module + pExportDir->AddressOfFunctions);
	const DWORD* pNames = (DWORD*)((BYTE*)module + pExportDir->AddressOfNames);
	const WORD* pOrdinals = (WORD*)((BYTE*)module + pExportDir->AddressOfNameOrdinals);

	auto index_to_proc = [=](size_t i) {
		return ProcEntry{
			(LPCSTR)((BYTE*)module + pNames[i]),
			(BYTE*)module + pFunctions[pOrdinals[i]],
		};
	};

	return std::views::iota(size_t(0), size_t(pExportDir->NumberOfNames)) |
		std::views::transform(index_to_proc);
}

inline const void* FindProc(const void* module, std::string_view proc_name)
{
	for (const auto& proc : ProcsView(module))
		if (proc.name == proc_name)
			return proc.func;

	return nullptr;
}

} // namespace win32_utils
