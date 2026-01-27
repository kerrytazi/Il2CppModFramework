#pragma once

#include "System/Reflection/MethodBase.hpp"

namespace il2cpp { template <typename T> class Array; }
namespace il2cpp { class Method; }
namespace System { class Type; }
namespace System { class String; }

namespace System::Reflection
{

class __autogen MethodInfo : public System::Reflection::MethodBase
{
public:

	__autogen_virtual System::Reflection::MethodInfo* MakeGenericMethod(il2cpp::Array<System::Type*>* typeArguments);

	const il2cpp::Method* ToIl2CppMethod() const { return method; }

private:

	const il2cpp::Method* method;
	System::String* name;
	struct Il2CppReflectionType* reftype;
};

} // namespace System::Reflection
