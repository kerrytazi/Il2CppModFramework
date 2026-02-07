#pragma once

#include "common/NoImplement.hpp"

#include "il2cpp/Type.hpp"
#include "il2cpp/Method.hpp"
#include "il2cpp/Field.hpp"
#include "il2cpp/ClassFinder.hpp"

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

	bool IsValueType() const;

	template <typename TMethod = Method>
	auto GetMethods() const
	{
		static_assert(std::is_same_v<TMethod, Method>, "nuh uh");
		return std::ranges::subrange(methods, methods + method_count);
	}

	template <typename TField = Field>
	auto GetFields() const
	{
		static_assert(std::is_same_v<TField, Field>, "nuh uh");
		return std::ranges::subrange(fields, fields + field_count)
			| std::views::transform([](const auto& v) { return &v; });
	}

	template <typename TProperty = Property>
	auto GetProperties() const
	{
		static_assert(std::is_same_v<TProperty, Property>, "nuh uh");
		return std::ranges::subrange(properties, properties + property_count)
			| std::views::transform([](const auto& v) { return &v; });
	}

	template <typename TEvent = Event>
	auto GetEvents() const
	{
		static_assert(std::is_same_v<TEvent, Event>, "nuh uh");
		return std::ranges::subrange(events, events + event_count)
			| std::views::transform([](const auto& v) { return &v; });
	}

	const Method* GetVirtualMethod(const Method* method) const;

	const Class* GetDeclaringClass() const;
	const Class* GetElementClass() const;
	const Class* GetBase() const;
	bool IsBaseOf(const Class* _derived) const;
	bool IsCastable(const Class* other) const;

	// Non-static only
	const Field* FindField(std::string_view field_name) const;
	const Field* FindFieldRecursive(std::string_view field_name) const;

	// Static only
	const Field* FindStaticField(std::string_view field_name) const;
	const Field* FindStaticFieldRecursive(std::string_view field_name) const;

	// Static literal only
	const Field* FindStaticLiteralField(std::string_view field_name) const;
	const Field* FindStaticLiteralFieldRecursive(std::string_view field_name) const;

	// Static thread local only
	const Field* FindStaticThreadLocalField(std::string_view field_name) const;
	const Field* FindStaticThreadLocalFieldRecursive(std::string_view field_name) const;

	const Method* FindMethod(std::string_view method_name) const
	{
		return _FindMethod(method_name);
	}

	const Method* FindMethod(std::string_view method_name, int param_count) const;

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

	// Slow!!!
	// Consider caching method search via CallCached.
	template <typename TRet, typename... TArgs>
	TRet DynamicStaticInvoke(std::string_view method_name, TArgs... args)
	{
		auto method = FindMethodRecursive(method_name);
		assert(method);

		return method->NativeInvoke<TRet, TArgs...>(args);
	}

	// Slow!!!
	// Consider caching result address via CallCached.
	template <typename T>
	T& DynamicStaticField(std::string_view field_name) const
	{
		assert(static_fields);

		// No recursive search.
		// Because each class have its own static_fields.
		auto field = FindStaticField(field_name);
		assert(field);

		return field->GetStatic<T>();
	}

	// Slow!!!
	// Consider caching result via CallCached.
	template <typename T>
	T DynamicStaticLiteralField(std::string_view field_name) const
	{
		// No recursive search.
		// Because each class have its own static_fields.
		auto field = FindStaticLiteralField(field_name);
		assert(field);

		T result;
		field->GetLiteral(&result);
		return result;
	}

	// Slow!!!
	// Consider caching result via CallCached.
	template <typename T>
	T DynamicStaticThreadLocalField(std::string_view field_name) const
	{
		// No recursive search.
		// Because each class have its own static_fields.
		auto field = FindStaticThreadLocalField(field_name);
		assert(field);

		T result;
		field->GetThreadLocal(&result);
		return result;
	}

	std::string BuildFullName() const;
	void BuildFullName(std::string& cache) const;

	// UNSAFE. Result stored statically and will be invalidated on next call.
	// Consider using BuildFullName instead.
	// Unless you absolutely understand what you are doing.
	const std::string& _BuildFullName() const;

	void* _GetStaticFields() const;

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

#if ICMF_UNITY_VERSION_NUM >= 2022308945
	uint32_t stack_slot_size; // valid when size_inited is true
#endif // ICMF_UNITY_VERSION_NUM >= 2022308945

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
#if ICMF_UNITY_VERSION_NUM <= 2021322258
	uint8_t naturalAligment; // Alignment of this type without accounting for packing
#endif // ICMF_UNITY_VERSION_NUM >= 2021322258
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
