#pragma once

#include <type_traits>
#include <optional>

template <typename TFunc>
class CallCachedStorage
{
public:

	using value_type = std::remove_reference_t<std::invoke_result_t<TFunc>>;

	value_type& Deref()
	{
		TryInit();
		return *value_;
	}

	void TryInit()
	{
		if (!value_.has_value()) [[unlikely]]
			value_ = TFunc{}();
	}

private:

	std::optional<value_type> value_;
};

void __RegisterCallCached(void(*init)());

template <typename TFunc>
struct __RegisterCallCachedStruct
{
	inline static bool initialized = []() {
		__RegisterCallCached([]() {
			CallCachedStorage<TFunc> storage;
			storage.TryInit();
		});

		return true;
	}();
};

void _CallCachedEverything();

template <typename TFunc>
auto CallCached(TFunc&&)
{
	(void)__RegisterCallCachedStruct<TFunc>{};
	CallCachedStorage<TFunc> storage;
	return storage.Deref();
}
