#pragma once

#include <mutex>

template <typename TValue, typename TMutex = std::mutex>
class LockedValue
{
public:

	LockedValue(TMutex& mtx, TValue& value)
		: lock_(mtx)
		, value_(value)
	{
	}

	TValue& operator*() const { return value_; }
	TValue* operator->() const { return &value_; }

private:

	std::unique_lock<TMutex> lock_;
	TValue& value_;
};
