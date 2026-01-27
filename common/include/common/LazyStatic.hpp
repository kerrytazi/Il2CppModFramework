#pragma once

#include <optional>

// Constructs object on first access.
// Useful when you can't control order of statics initialization.
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
