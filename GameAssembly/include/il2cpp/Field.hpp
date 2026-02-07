#pragma once

#include "common/NoImplement.hpp"

#include "il2cpp/Type.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace il2cpp
{

class Class;

class Field : _NoImplement
{
public:

	const Class* GetParent() const { return parent; }
	std::string_view GetName() const { return name; } // null-terminated
	size_t GetOffset() const { return offset; }
	const Type* GetType() const { return type; }

	template <typename TClass>
	bool IsTypeClass() const
	{
		return GetType()->IsTypeClass<TClass>();
	}

	bool IsInstance() const { return !IsStatic(); }
	bool IsStatic() const;
	bool IsLiteral() const;
	bool IsThreadLocal() const;

	void* GetInstance(void* instance) const
	{
		assert(IsInstance());
		return reinterpret_cast<uint8_t*>(instance) + GetOffset();
	}

	template <typename T>
	T& GetInstance(void* instance) const
	{
		assert(IsInstance());
		return *reinterpret_cast<T*>(GetInstance(instance));
	}

	template <typename T>
	T& GetStatic() const
	{
		assert(IsStatic());
		assert("field is literal" && !IsLiteral());
		assert("field is thread local" && !IsThreadLocal());

		return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(_GetParentStaticFields()) + GetOffset());
	}

	void GetLiteral(void* out) const;
	void GetThreadLocal(void* out) const;

private:

	void* _GetParentStaticFields() const;

	const char* name;
	const Type* type;
	Class* parent;
	int32_t offset; // If offset is -1, then it's thread static
	uint32_t token;

	static const uint16_t FIELD_ATTRIBUTE_FIELD_ACCESS_MASK     = 0x0007;
	static const uint16_t FIELD_ATTRIBUTE_COMPILER_CONTROLLED   = 0x0000;
	static const uint16_t FIELD_ATTRIBUTE_PRIVATE               = 0x0001;
	static const uint16_t FIELD_ATTRIBUTE_FAM_AND_ASSEM         = 0x0002;
	static const uint16_t FIELD_ATTRIBUTE_ASSEMBLY              = 0x0003;
	static const uint16_t FIELD_ATTRIBUTE_FAMILY                = 0x0004;
	static const uint16_t FIELD_ATTRIBUTE_FAM_OR_ASSEM          = 0x0005;
	static const uint16_t FIELD_ATTRIBUTE_PUBLIC                = 0x0006;
	static const uint16_t FIELD_ATTRIBUTE_STATIC                = 0x0010;
	static const uint16_t FIELD_ATTRIBUTE_INIT_ONLY             = 0x0020;
	static const uint16_t FIELD_ATTRIBUTE_LITERAL               = 0x0040;
	static const uint16_t FIELD_ATTRIBUTE_NOT_SERIALIZED        = 0x0080;
	static const uint16_t FIELD_ATTRIBUTE_SPECIAL_NAME          = 0x0200;
	static const uint16_t FIELD_ATTRIBUTE_PINVOKE_IMPL          = 0x2000;

	/* For runtime use only */
	static const uint16_t FIELD_ATTRIBUTE_RESERVED_MASK         = 0x9500;
	static const uint16_t FIELD_ATTRIBUTE_RT_SPECIAL_NAME       = 0x0400;
	static const uint16_t FIELD_ATTRIBUTE_HAS_FIELD_MARSHAL     = 0x1000;
	static const uint16_t FIELD_ATTRIBUTE_HAS_DEFAULT           = 0x8000;
	static const uint16_t FIELD_ATTRIBUTE_HAS_FIELD_RVA         = 0x0100;
};

} // namespace il2cpp
