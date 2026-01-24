#pragma once

#include "System/Reflection/MemberInfo.hpp"

namespace il2cpp { template <typename T> class Array; }
namespace il2cpp { class Type; }

namespace System
{

class __autogen Type : public System::Reflection::MemberInfo
{
public:

	__autogen_virtual System::Type* MakeGenericType(il2cpp::Array<System::Type*>* typeArguments);

	const il2cpp::Type* ToIl2CppType() const { return type; }

private:

	const il2cpp::Type* type;
};

} // namespace System
