#include "pch.hpp"

#include "common/CallCached.hpp"
#include "common/LazyStatic.hpp"

static LazyStatic<std::vector<void(*)()>> g_registered_call_cached;

void __RegisterCallCached(void(*init)())
{
	g_registered_call_cached->push_back(init);
}

void _CallCachedEverything()
{
	for (auto init : *g_registered_call_cached)
		init();
}
