#pragma once

#include "il2cpp/gc_ref.hpp"
#include "_tmp_method_info.hpp"
#include "System/Object.hpp"
#include "common/CallCached.hpp"

#include <vector>
#include <memory>
#include <ranges>
#include <algorithm>

struct WeakMethodInfo
{
	WeakMethodInfo(System::Object* _obj, il2cpp::TmpMethodInfo* _method)
		: obj{ _obj }
		, method{ _method }
	{
	}

	~WeakMethodInfo()
	{
		delete method;
	}

	bool IsAlive() const { return obj.is_alive(); }

	il2cpp::gc_ref<System::Object> obj;
	il2cpp::TmpMethodInfo* method;
};

inline std::vector<std::unique_ptr<WeakMethodInfo>> g_method_infos;

inline il2cpp::TmpMethodInfo* _AllocMethodInfo()
{
	auto method = new il2cpp::TmpMethodInfo{};
	method->name = "";
	method->klass = (il2cpp::Class*)CallCached([]() {
		auto method_klass = il2cpp::Class::Find("", "<>c");
		assert(method_klass);
		return method_klass;
	});
	return method;
}

inline void __RegisterTmpMethodInfo(System::Object* obj, void* method)
{
	{
		auto removed = std::ranges::remove_if(g_method_infos, [](const auto& ptr) { return !ptr->IsAlive(); });
		g_method_infos.erase(removed.begin(), removed.end());
	}

	g_method_infos.push_back(std::make_unique<WeakMethodInfo>(obj, (il2cpp::TmpMethodInfo*)method));
}
