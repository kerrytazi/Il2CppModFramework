#pragma once

#include "common/NoImplement.hpp"

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace il2cpp
{

class Type;
class Class;
class MethodInfo;

class Event : NoImplement
{
public:

	const Class* GetParent() const { return parent; }
	std::string_view GetName() const { return name; } // null-terminated

private:

	const char* name;
	const Type* eventType;
	Class* parent;
	const MethodInfo* add;
	const MethodInfo* remove;
	const MethodInfo* raise;
	uint32_t token;
};

} // namespace il2cpp
