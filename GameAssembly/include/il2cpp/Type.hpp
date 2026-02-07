#pragma once

#include "common/NoImplement.hpp"

#include "il2cpp/ClassFinder.hpp"
#include "System/primitives.hpp"

#include <string>
#include <array>

namespace System { class Type; }
namespace System { class String; }

namespace il2cpp
{

class Class;
class Field;

class Type : _NoImplement
{
public:

	std::string GetName() const;

	// USAFE. Reference may be invalid on consequent call
	const std::string& _GetName() const;

	int SizeOfType() const; // 0 = void, -1 = unknown
	System::Type* ToReflectionType() const;
	const Class* ToClass() const;

	template <typename TClass>
	bool IsTypeClass() const
	{
		if constexpr (std::is_same_v<TClass, System::Void>)
			return type == Il2CppTypeEnum::Void;
		if constexpr (std::is_same_v<TClass, System::Boolean>)
			return type == Il2CppTypeEnum::Boolean;
		if constexpr (std::is_same_v<TClass, System::Char>)
			return type == Il2CppTypeEnum::Char;
		if constexpr (std::is_same_v<TClass, System::SByte>)
			return type == Il2CppTypeEnum::I1;
		if constexpr (std::is_same_v<TClass, System::Byte>)
			return type == Il2CppTypeEnum::U1;
		if constexpr (std::is_same_v<TClass, System::Int16>)
			return type == Il2CppTypeEnum::I2;
		if constexpr (std::is_same_v<TClass, System::UInt16>)
			return type == Il2CppTypeEnum::U2;
		if constexpr (std::is_same_v<TClass, System::Int32>)
			return type == Il2CppTypeEnum::I4;
		if constexpr (std::is_same_v<TClass, System::UInt32>)
			return type == Il2CppTypeEnum::U4;
		if constexpr (std::is_same_v<TClass, System::Int64>)
			return type == Il2CppTypeEnum::I8;
		if constexpr (std::is_same_v<TClass, System::UInt64>)
			return type == Il2CppTypeEnum::U8;
		if constexpr (std::is_same_v<TClass, System::Single>)
			return type == Il2CppTypeEnum::R4;
		if constexpr (std::is_same_v<TClass, System::Double>)
			return type == Il2CppTypeEnum::R8;
		if constexpr (std::is_same_v<TClass, System::IntPtr>)
			return type == Il2CppTypeEnum::I;
		if constexpr (std::is_same_v<TClass, System::UIntPtr>)
			return type == Il2CppTypeEnum::U;
		if constexpr (std::is_same_v<TClass, System::String>)
			return type == Il2CppTypeEnum::String;

		return ToClass() == il2cpp::Find<TClass>();
	}

	bool IsTypeFastSearch() const
	{
		static constexpr std::array<bool, 64> FAST_CHECK = []() {
			std::array<bool, 64> result{};

			result[size_t(Il2CppTypeEnum::Void)] = true;
			result[size_t(Il2CppTypeEnum::Boolean)] = true;
			result[size_t(Il2CppTypeEnum::Char)] = true;
			result[size_t(Il2CppTypeEnum::I1)] = true;
			result[size_t(Il2CppTypeEnum::U1)] = true;
			result[size_t(Il2CppTypeEnum::I2)] = true;
			result[size_t(Il2CppTypeEnum::U2)] = true;
			result[size_t(Il2CppTypeEnum::I4)] = true;
			result[size_t(Il2CppTypeEnum::U4)] = true;
			result[size_t(Il2CppTypeEnum::I8)] = true;
			result[size_t(Il2CppTypeEnum::U8)] = true;
			result[size_t(Il2CppTypeEnum::R4)] = true;
			result[size_t(Il2CppTypeEnum::R8)] = true;
			result[size_t(Il2CppTypeEnum::String)] = true;

			result[size_t(Il2CppTypeEnum::I)] = true;
			result[size_t(Il2CppTypeEnum::U)] = true;

			return result;
		}();

		if ((size_t)type < FAST_CHECK.size())
			return FAST_CHECK[(size_t)type];

		return false;
	}

	bool IsValueType() const { return valuetype; }

private:

	friend class Field;

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
