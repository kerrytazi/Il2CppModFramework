#include "pch.hpp"

#include "System/primitives.hpp"

#include "il2cpp/ClassFinder.hpp"
#include "il2cpp/Class.hpp"
#include "common/CallCached.hpp"

#define FINDER_PRIMITIVE(_Type) \
const il2cpp::Class* il2cpp::FindClassOnce<System::_Type>::Find() \
{ \
	auto klass = CallCached<decltype([]() { return il2cpp::Class::Find("System", #_Type); })>(); assert(klass); \
	klass->_ForceInitFull(); \
	return klass; \
}

FINDER_PRIMITIVE(Void);
FINDER_PRIMITIVE(Boolean);
FINDER_PRIMITIVE(Char);
FINDER_PRIMITIVE(Byte);
FINDER_PRIMITIVE(SByte);
FINDER_PRIMITIVE(Int16);
FINDER_PRIMITIVE(UInt16);
FINDER_PRIMITIVE(Int32);
FINDER_PRIMITIVE(UInt32);
FINDER_PRIMITIVE(Int64);
FINDER_PRIMITIVE(UInt64);
FINDER_PRIMITIVE(Single);
FINDER_PRIMITIVE(Double);
FINDER_PRIMITIVE(IntPtr);
FINDER_PRIMITIVE(UIntPtr);
