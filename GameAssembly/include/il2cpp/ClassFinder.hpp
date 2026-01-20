#pragma once

namespace il2cpp
{

class Class;

template <typename TClass>
struct FindClassOnce
{
	static const Class* Find();
};

} // namespace il2cpp
