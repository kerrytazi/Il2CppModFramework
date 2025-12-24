#pragma once

#include "common/NoImplement.hpp"

namespace il2cpp { class Class; }

namespace System
{

class Object : NoImplement
{
public:

	const il2cpp::Class* GetClass() { return klass; }

private:

	const il2cpp::Class* klass;
	struct Il2CppMonitorData* monitor;
};

} // namespace System
