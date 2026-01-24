#pragma once

#include "common/NoImplement.hpp"
#include "common/autogen.hpp"

namespace il2cpp { class Class; }

namespace System
{

class String;

class __autogen Object : _NoImplement
{
public:

	__autogen_virtual System::String* ToString();

	const il2cpp::Class* GetClass() { return klass; }

private:

	const il2cpp::Class* klass;
	struct Il2CppMonitorData* monitor;
};

} // namespace System
