#pragma once

#include <utility>
#include <type_traits>

template <typename T, typename TFunc>
class LazyInit
{
public:

	LazyInit(TFunc&& func)
		: func_{ std::move(func) }
	{
	}

	T& Deref()
	{
		CheckInit();
		return *value_;
	}

	T& operator*()
	{
		return Deref();
	}

	T* operator->()
	{
		return &Deref();
	}

private:

	void CheckInit()
	{
		if (!value_.has_value())
			value_ = func_();
	}

	std::optional<T> value_;
	TFunc func_;
};

template <typename TFunc>
LazyInit(TFunc) -> LazyInit<std::invoke_result_t<TFunc>, TFunc>;
