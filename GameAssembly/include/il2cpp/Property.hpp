#pragma once

#include "common/NoImplement.hpp"

#include <string_view>

namespace il2cpp
{

class Class;
class Method;

class Property : NoImplement
{
public:

	const Class* GetParent() const { return parent; }
	std::string_view GetName() const { return name; } // null-terminated
	const Method* GetGetMethod() const { return get; }
	const Method* GetSetMethod() const { return set; }

private:

	Class* parent;
	const char* name;
	const Method* get;
	const Method* set;
	uint32_t attrs;
	uint32_t token;
};

} // namespace il2cpp
