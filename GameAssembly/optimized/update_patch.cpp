#include "il2cpp/Method.hpp"

#include "common/ModuleManager.hpp"
#include "common/Log.hpp"
#include "common/SEHGuarded.hpp"
#include "il2cpp/il2cpp.hpp"

#include "static_lambda/detour_lambda.hpp"

#include <stdexcept>
#include <ranges>

static std::optional<sl::detour<int64_t()>> g_player_loop_detour;

struct FriendIl2CppOnUpdate
{
	static void PatchPlayerLoop(int64_t(*update_func)())
	{
		g_player_loop_detour.emplace(update_func, [](auto original) {

			if (g_module_manager)
				g_module_manager->OnPreUpdate();

			auto result = original();

			if (g_module_manager)
				g_module_manager->OnPostUpdate();

			return result;
		});
	}
};

static bool PatchUnityPlayerLoop()
{
	auto original_player_loop = SEHGuarded("PatchUnityPlayerLoop", []() -> const void* {
		const char err_message[] =
			"An abnormal situation has occurred: "
			"the PlayerLoop internal function has been called recursively. "
			"Please contact Customer Support with a sample project "
			"so that we can reproduce the problem and troubleshoot it.";

		auto UnityPlayer = reinterpret_cast<const char*>(il2cpp::GetUnityPlayerBase());

		const char* err_message_begin = std::ranges::search(UnityPlayer, (const char*)-1, std::begin(err_message), std::end(err_message)).begin();
		const char* lea_err_message = nullptr;

		for (const char* it = UnityPlayer; ; ++it)
		{
			// 48 8D 05 <immediate>   lea rax, [err_message_begin]
			int immediate = int(err_message_begin - it) - 4;

			if (memcmp(it, &immediate, 4) == 0)
			{
				lea_err_message = it;
				break;
			}
		}

		if (lea_err_message == nullptr)
			return nullptr;

		auto find_instruction_up = [&](std::initializer_list<uint8_t> instr) -> const char* {
			for (const char* it = lea_err_message; it != lea_err_message - 1024; --it)
				if (memcmp(it, instr.begin(), instr.size()) == 0)
					return it;

			return nullptr;
		};

		// 40 55   push rbp
		auto push_rbp = find_instruction_up({ 0x40, 0x55 });

		// 48 83 EC XX  sub rsp, <imm>
		auto sub_rsp = find_instruction_up({ 0x48, 0x83, 0xEC });

		return (sub_rsp < push_rbp) ? push_rbp : sub_rsp;
	});

	if (!original_player_loop.has_value())
	{
		Log::Error("PatchUnityPlayerLoop: Memory access violation");
		return false;
	}

	if (*original_player_loop == nullptr)
	{
		Log::Error("PatchUnityPlayerLoop: Can't find original function");
		return false;
	}

	FriendIl2CppOnUpdate::PatchPlayerLoop((int64_t(*)())*original_player_loop);

	return true;
}

void _Il2CppPatchUpdate()
{
	Log::Debug("Il2CppPatchUpdate");

	g_module_manager->RequestLoad();

	if (!PatchUnityPlayerLoop())
		Log::Error("PatchUnityPlayerLoop failed");
}

void _Il2CppDisableUpdate()
{
	Log::Debug("_Il2CppDisableUpdate");
	g_player_loop_detour.reset();
}
