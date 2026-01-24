#pragma once

#include "common/NoImplement.hpp"

#include <string>

namespace System { class Type; }

namespace il2cpp
{

class Class;

class Type : _NoImplement
{
public:

	const std::string& GetName() const; // Reference may be invalid on consequent call
	int SizeOfType() const; // 0 = void, -1 = unknown
	System::Type* ToSystemType() const;
	const Class* ToClass() const;

private:

	enum class Il2CppTypeEnum
	{
		// END = 0x00,       /* End of List */
		Void = 0x01,
		Boolean = 0x02,
		Char = 0x03,
		I1 = 0x04,
		U1 = 0x05,
		I2 = 0x06,
		U2 = 0x07,
		I4 = 0x08,
		U4 = 0x09,
		I8 = 0x0a,
		U8 = 0x0b,
		R4 = 0x0c,
		R8 = 0x0d,
		String = 0x0e,
		Ptr = 0x0f,       /* arg: <type> token */
		ByRef = 0x10,       /* arg: <type> token */
		ValueType = 0x11,       /* arg: <type> token */
		Class = 0x12,       /* arg: <type> token */
		Var = 0x13,       /* Generic parameter in a generic type definition, represented as number (compressed unsigned integer) number */
		Array = 0x14,       /* type, rank, boundsCount, bound1, loCount, lo1 */
		GenericInst = 0x15,     /* <type> <type-arg-count> <type-1> \x{2026} <type-n> */
		TypedByRef = 0x16,
		I = 0x18,
		U = 0x19,
		FnPtr = 0x1b,        /* arg: full method signature */
		Object = 0x1c,
		SzArray = 0x1d,       /* 0-based one-dim-array */
		MVar = 0x1e,       /* Generic parameter in a generic method definition, represented as number (compressed unsigned integer)  */
		CMOD_REQD = 0x1f,       /* arg: typedef or typeref token */
		CMOD_OPT = 0x20,       /* optional arg: typedef or typref token */
		INTERNAL = 0x21,       /* CLR internal type */

		MODIFIER = 0x40,       /* Or with the following types */
		SENTINEL = 0x41,       /* Sentinel for varargs method signature */
		PINNED = 0x45,       /* Local var that points to pinned object */

		ENUM = 0x55,        /* an enumeration */
		// _TYPE_INDEX = 0xff        /* an index into  type metadata table */
	};

	using Il2CppTypeDefinitionIndex = int32_t;
	using Il2CppGenericParameterIndex = int32_t;

	union
	{
		// We have this dummy field first because pre C99 compilers (MSVC) can only initializer the first value in a union.
		void* dummy;
		Il2CppTypeDefinitionIndex __klassIndex; /* for VALUETYPE and CLASS at startup */
		const struct ___Il2CppMetadataTypeHandle* typeHandle; /* for VALUETYPE and CLASS at runtime */
		const Type* type;   /* for PTR and SZARRAY */
		struct Il2CppArrayType* array; /* for ARRAY */
		//MonoMethodSignature *method;
		Il2CppGenericParameterIndex __genericParameterIndex; /* for VAR and MVAR at startup */
		const struct ___Il2CppMetadataGenericParameterHandle* genericParameterHandle; /* for VAR and MVAR at runtime */
		struct Il2CppGenericClass* generic_class; /* for GENERICINST */
	} data;
	unsigned int attrs : 16; /* param attributes or field flags */
	Il2CppTypeEnum type : 8;
	unsigned int num_mods : 5;  /* max 64 modifiers follow at the end */
	unsigned int byref : 1;
	unsigned int pinned : 1;  /* valid when included in a local var signature */
	unsigned int valuetype : 1;
	//MonoCustomMod modifiers [MONO_ZERO_LEN_ARRAY]; /* this may grow */
};

} // namespace il2cpp
