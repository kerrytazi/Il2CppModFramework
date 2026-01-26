#pragma once

#include "common/NoImplement.hpp"

#include "il2cpp/Type.hpp"
#include "il2cpp/Method.hpp"
#include "il2cpp/Field.hpp"

#include <string_view>
#include <optional>
#include <span>
#include <cassert>

namespace il2cpp
{

class Image;
class Method;
class Field;
class Property;
class Event;

class Class : _NoImplement
{
public:

	const Type* GetType() const;
	std::string_view GetName() const; // null-terminated
	std::string_view GetNamespace() const; // null-terminated

	std::span<const Method*> GetMethods() const;
	std::span<const Field> GetFields() const;
	std::span<const Property> GetProperties() const;
	std::span<const Event> GetEvents() const;

	const Method* GetVirtualMethod(const Method* method) const;

	const Class* GetDeclaringClass() const;
	const Class* GetElementClass() const;
	const Class* GetBase() const;
	const bool IsBaseOf(const Class* _derived) const;

	// Slow!!!
	// Consider caching method search via CallCached.
	template <typename TRet, typename... TArgs>
	TRet DynamicStaticInvoke(std::string_view method_name, TArgs... args)
	{
		auto method = FindMethodRecursive(method_name);
		assert(method);

		auto method_ptr = method->GetMethodPointer<TRet(std::remove_reference_t<TArgs>...)>();
		assert(method_ptr);

		return method_ptr(args...);
	}

	const Method* FindMethod(std::string_view method_name) const
	{
		return _FindMethod(method_name);
	}

	const Method* FindMethod(
		std::string_view method_name,
		std::string_view ret_type) const
	{
		return _FindMethod(method_name, ret_type);
	}

	const Method* FindMethod(
		std::string_view method_name,
		std::string_view ret_type,
		std::initializer_list<std::string_view> param_types) const
	{
		return _FindMethod(method_name, ret_type, param_types);
	}

	const Method* FindMethod(
		std::string_view method_name,
		std::string_view ret_type,
		std::initializer_list<std::string_view> param_types,
		bool is_static) const
	{
		return _FindMethod(method_name, ret_type, param_types, is_static);
	}

	const Method* FindMethodRecursive(std::string_view method_name) const
	{
		return _FindMethodRecursive(method_name);
	}

	const Method* FindMethodRecursive(
		std::string_view method_name,
		std::string_view ret_type) const
	{
		return _FindMethodRecursive(method_name, ret_type);
	}

	const Method* FindMethodRecursive(
		std::string_view method_name,
		std::string_view ret_type,
		std::initializer_list<std::string_view> param_types) const
	{
		return _FindMethodRecursive(method_name, ret_type, param_types);
	}

	const Method* FindMethodRecursive(
		std::string_view method_name,
		std::string_view ret_type,
		std::initializer_list<std::string_view> param_types,
		bool is_static) const
	{
		return _FindMethodRecursive(method_name, ret_type, param_types, is_static);
	}

	static const Class* Find(std::string_view namespaze, std::string_view class_name);

	void _ForceInitFull() const;
	void _ForceInitMethods() const;
	void _ForceInitFields() const;
	void _ForceInitProperties() const;
	void _ForceInitEvents() const;

private:

	const Method* _FindMethod(
		std::string_view method_name,
		std::optional<std::string_view> ret_type = std::nullopt,
		std::optional<std::initializer_list<std::string_view>> param_types = std::nullopt,
		std::optional<bool> is_static = std::nullopt) const;

	const Method* _FindMethodRecursive(
		std::string_view method_name,
		std::optional<std::string_view> ret_type = std::nullopt,
		std::optional<std::initializer_list<std::string_view>> param_types = std::nullopt,
		std::optional<bool> is_static = std::nullopt) const;

	struct Il2CppVirtualInvokeData
	{
		void* methodPtr;
		const Method* method;
	};
	
	// The following fields are always valid for a Class structure
	const Image* image;
	void* gc_desc;
	const char* name;
	const char* namespaze;
	Type byval_arg;
	Type this_arg;
	Class* element_class;
	Class* castClass;
	Class* declaringType;
	Class* parent;
	struct Il2CppGenericClass* generic_class;
	const struct ___Il2CppMetadataTypeHandle* typeMetadataHandle; // non-NULL for Class's constructed from type definitions
	const struct Il2CppInteropData* interopData;
	Class* klass; // hack to pretend we are a MonoVTable. Points to ourself
	// End always valid fields

	// The following fields need initialized before access. This can be done per field or as an aggregate via a call to Class::Init
	const Field* fields; // Initialized in SetupFields
	const Event* events; // Initialized in SetupEvents
	const Property* properties; // Initialized in SetupProperties
	const Method** methods; // Initialized in SetupMethods
	Class** nestedTypes; // Initialized in SetupNestedTypes
	Class** implementedInterfaces; // Initialized in SetupInterfaces
	struct Il2CppRuntimeInterfaceOffsetPair* interfaceOffsets; // Initialized in Init
	void* static_fields; // Initialized in Init
	const struct Il2CppRGCTXData* rgctx_data; // Initialized in Init
	// used for fast parent checks
	Class** typeHierarchy; // Initialized in SetupTypeHierachy
	// End initialization required fields

	void* unity_user_data;

	uint32_t initializationExceptionGCHandle;

	uint32_t cctor_started;
	uint32_t cctor_finished_or_no_cctor;
	size_t cctor_thread;

	// Remaining fields are always valid except where noted
	const struct ___Il2CppMetadataGenericContainerHandle* genericContainerHandle;
	uint32_t instance_size; // valid when size_inited is true

#if UC_UNITY_VERSION_NUM >= 2022308945
	uint32_t stack_slot_size; // valid when size_inited is true
#endif // UC_UNITY_VERSION_NUM >= 2022308945

	uint32_t actualSize;
	uint32_t element_size;
	int32_t native_size;
	uint32_t static_fields_size;
	uint32_t thread_static_fields_size;
	int32_t thread_static_fields_offset;
	uint32_t flags;
	uint32_t token;

	uint16_t method_count; // lazily calculated for arrays, i.e. when rank > 0
	uint16_t property_count;
	uint16_t field_count;
	uint16_t event_count;
	uint16_t nested_type_count;
	uint16_t vtable_count; // lazily calculated for arrays, i.e. when rank > 0
	uint16_t interfaces_count;
	uint16_t interface_offsets_count; // lazily calculated for arrays, i.e. when rank > 0

	uint8_t typeHierarchyDepth; // Initialized in SetupTypeHierachy
	uint8_t genericRecursionDepth;
	uint8_t rank;
	uint8_t minimumAlignment; // Alignment of this type
#if UC_UNITY_VERSION_NUM <= 2021322258
	uint8_t naturalAligment; // Alignment of this type without accounting for packing
#endif // UC_UNITY_VERSION_NUM >= 2021322258
	uint8_t packingSize;

	// this is critical for performance of Class::InitFromCodegen. Equals to initialized && !initializationExceptionGCHandle at all times.
	// Use Class::PublishInitialized to update
	uint8_t initialized_and_no_error : 1;

	uint8_t initialized : 1;
	uint8_t enumtype : 1;
	uint8_t nullabletype : 1;
	uint8_t is_generic : 1;
	uint8_t has_references : 1; // valid when size_inited is true
	uint8_t init_pending : 1;
	uint8_t size_init_pending : 1;
	uint8_t size_inited : 1;
	uint8_t has_finalize : 1;
	uint8_t has_cctor : 1;
	uint8_t is_blittable : 1;
	uint8_t is_import_or_windows_runtime : 1;
	uint8_t is_vtable_initialized : 1;
	uint8_t is_byref_like : 1;
	Il2CppVirtualInvokeData vtable[1]; // variable size
};

} // namespace il2cpp
