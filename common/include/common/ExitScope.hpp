#pragma once

#include <utility>

template <typename TFunc>
class ExitScope
{
public:

	ExitScope(TFunc&& func)
		: func_{ std::move(func) }
	{
	}

	~ExitScope()
	{
		func_();
	}

	ExitScope(const ExitScope&) = delete;
	ExitScope& operator=(const ExitScope&) = delete;

private:

	TFunc func_;
};
