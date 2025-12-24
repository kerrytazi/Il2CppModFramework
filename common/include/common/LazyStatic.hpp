#pragma once

#include <optional>

template <typename T>
class LazyStatic
{
public:

	T& SafeDeref()
	{
		if (!val_) [[unlikely]]
			val_ = T{};

		return *val_;
	}

	const T& SafeDeref() const
	{
		if (!val_) [[unlikely]]
			val_ = T{};

		return *val_;
	}

	T& operator*() { return SafeDeref(); }
	const T& operator*() const { return SafeDeref(); }

	T* operator->() { return &SafeDeref(); }
	const T* operator->() const { return &SafeDeref(); }

private:

	std::optional<T> val_;
};
