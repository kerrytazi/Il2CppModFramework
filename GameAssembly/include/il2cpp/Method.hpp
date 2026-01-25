#pragma once

#include "common/NoImplement.hpp"
#include "common/Templates.hpp"

#include <initializer_list>
#include <string_view>
#include <ranges>
#include <optional>

namespace il2cpp
{

class Class;
class Type;

class Method : _NoImplement
{
public:

	const Class* GetClass() const { return klass; }
	std::string_view GetName() const { return name; } // null-terminated

	bool IsStatic() const { return flags & METHOD_ATTRIBUTE_STATIC; }
	bool IsFinal() const { return flags & METHOD_ATTRIBUTE_FINAL; }
	bool IsVirtual() const { return flags & METHOD_ATTRIBUTE_VIRTUAL; }
	bool IsPublic() const { return (flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PUBLIC; }
	bool IsPrivate() const { return (flags & METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK) == METHOD_ATTRIBUTE_PRIVATE; }

	uint16_t GetVirtualMethodSlot() const;

	const Type* GetReturnType() const { return return_type; }

	size_t GetParametersCount() const { return parameters_count; }
	auto GetParameterTypesView() const
	{
		const auto get_paramter_by_index = [this](size_t index) {
			return (const Type*)parameters[index];
		};

		return
			std::views::iota(size_t(0), GetParametersCount()) |
			std::views::transform(get_paramter_by_index);
	}

	void* _GetMethodPointer() const { return (void*)methodPointer; }

	template <typename TFunc>
	auto GetMethodPointer() const { return reinterpret_cast<typename templates::func<TFunc>::pointer>(_GetMethodPointer()); }

private:

	using Il2CppMethodPointer = void(*)();
	using InvokerMethod = void(*)(Il2CppMethodPointer, const Method*, void*, void**, void*);

	Il2CppMethodPointer methodPointer;
	Il2CppMethodPointer virtualMethodPointer;
	InvokerMethod invoker_method;
	const char* name;
	Class* klass;
	const Type* return_type;
	const Type** parameters;

	union
	{
		const struct Il2CppRGCTXData* rgctx_data; // is_inflated is true and is_generic is false, i.e. a generic instance method
		const struct ___Il2CppMetadataMethodHandle* methodMetadataHandle;
	} _u1;

	// note, when is_generic == true and is_inflated == true the method represents an uninflated generic method on an inflated type.
	union
	{
		const struct Il2CppGenericMethod* genericMethod; // is_inflated is true
		const struct ___Il2CppMetadataGenericContainerHandle* genericContainerHandle; // is_inflated is false and is_generic is true
	} _u2;

	uint32_t token;
	uint16_t flags;
	uint16_t iflags;
	uint16_t slot;
	uint8_t parameters_count;
	uint8_t is_generic : 1; // true if method is a generic method definition
	uint8_t is_inflated : 1; // true if declaring_type is a generic instance or if method is a generic instance
	uint8_t wrapper_type : 1; // always zero (MONO_WRAPPER_NONE) needed for the debugger
	uint8_t has_full_generic_sharing_signature : 1;

	const static uint16_t METHOD_IMPL_ATTRIBUTE_CODE_TYPE_MASK =       0x0003;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_IL =                   0x0000;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_NATIVE =               0x0001;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_OPTIL =                0x0002;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_RUNTIME =              0x0003;

	const static uint16_t METHOD_IMPL_ATTRIBUTE_MANAGED_MASK =         0x0004;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_UNMANAGED =            0x0004;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_MANAGED =              0x0000;

	const static uint16_t METHOD_IMPL_ATTRIBUTE_FORWARD_REF =          0x0010;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_PRESERVE_SIG =         0x0080;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL =        0x1000;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_SYNCHRONIZED =         0x0020;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_NOINLINING =           0x0008;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_NOOPTIMIZATION =       0x0040;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_MAX_METHOD_IMPL_VAL =  0xffff;
	const static uint16_t METHOD_IMPL_ATTRIBUTE_AGGRESSIVE_INLINING =  0x0100;

	const static uint16_t METHOD_ATTRIBUTE_MEMBER_ACCESS_MASK =        0x0007;
	const static uint16_t METHOD_ATTRIBUTE_COMPILER_CONTROLLED =       0x0000;
	const static uint16_t METHOD_ATTRIBUTE_PRIVATE =                   0x0001;
	const static uint16_t METHOD_ATTRIBUTE_FAM_AND_ASSEM =             0x0002;
	const static uint16_t METHOD_ATTRIBUTE_ASSEM =                     0x0003;
	const static uint16_t METHOD_ATTRIBUTE_FAMILY =                    0x0004;
	const static uint16_t METHOD_ATTRIBUTE_FAM_OR_ASSEM =              0x0005;
	const static uint16_t METHOD_ATTRIBUTE_PUBLIC =                    0x0006;

	const static uint16_t METHOD_ATTRIBUTE_STATIC =                    0x0010;
	const static uint16_t METHOD_ATTRIBUTE_FINAL =                     0x0020;
	const static uint16_t METHOD_ATTRIBUTE_VIRTUAL =                   0x0040;
	const static uint16_t METHOD_ATTRIBUTE_HIDE_BY_SIG =               0x0080;

	const static uint16_t METHOD_ATTRIBUTE_VTABLE_LAYOUT_MASK =        0x0100;
	const static uint16_t METHOD_ATTRIBUTE_REUSE_SLOT =                0x0000;
	const static uint16_t METHOD_ATTRIBUTE_NEW_SLOT =                  0x0100;

	const static uint16_t METHOD_ATTRIBUTE_STRICT =                    0x0200;
	const static uint16_t METHOD_ATTRIBUTE_ABSTRACT =                  0x0400;
	const static uint16_t METHOD_ATTRIBUTE_SPECIAL_NAME =              0x0800;

	const static uint16_t METHOD_ATTRIBUTE_PINVOKE_IMPL =              0x2000;
	const static uint16_t METHOD_ATTRIBUTE_UNMANAGED_EXPORT =          0x0008;

	const static uint16_t METHOD_ATTRIBUTE_REQSECOBJ =                 0x8000;

	const static uint16_t METHOD_ATTRIBUTE_RESERVED_MASK =             0xd000;
	const static uint16_t METHOD_ATTRIBUTE_RT_SPECIAL_NAME =           0x1000;
	const static uint16_t METHOD_ATTRIBUTE_HAS_SECURITY =              0x4000;
	const static uint16_t METHOD_ATTRIBUTE_REQUIRE_SEC_OBJECT =        0x8000;
};

} // namespace il2cpp
