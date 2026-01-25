#pragma once

namespace il2cpp
{

class Class;

template <typename TClass>
struct FindClassOnce
{
	static const Class* Find();
};

template <typename TClass>
inline const Class* Find()
{
	return FindClassOnce<TClass>::Find();
}

} // namespace il2cpp
