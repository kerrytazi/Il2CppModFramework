#pragma once

#include "common/NoImplement.hpp"
#include "common/autogen.hpp"

#include "il2cpp/ClassFinder.hpp"
#include "il2cpp/Class.hpp"

namespace System
{

class String;

class __autogen Object : _NoImplement
{
public:

	__autogen_virtual System::String* ToString();

	const il2cpp::Class* GetClass() const { return klass; }

	Object* TryDownCast(const il2cpp::Class* derived)
	{
		if (GetClass()->IsBaseOf(derived))
			return this;

		return nullptr;
	}

	template <typename TClass>
	TClass* TryDownCast()
	{
		return static_cast<TClass*>(TryDownCast(il2cpp::Find<TClass>()));
	}

	// Slow!!!
	// Consider caching method search via CallCached.
	template <typename TRet, typename... TArgs>
	TRet DynamicInvoke(std::string_view method_name, TArgs... args)
	{
		auto method = klass->FindMethodRecursive(method_name);
		assert(method);

		if (method->IsVirtual())
			method = klass->GetVirtualMethod(method);

		auto method_ptr = method->GetMethodPointer<TRet(System::Object*, std::remove_reference_t<TArgs>...)>();
		assert(method_ptr);

		return method_ptr(this, args...);
	}

	// Slow!!!
	// Consider declaring fields in your class.
	template <typename T>
	T& DynamicField(std::string_view field_name)
	{
		auto field = klass->FindFieldRecursive(field_name);
		assert(field);

		return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(this) + field->GetOffset());
	}

	struct _GetFieldResult
	{
		const il2cpp::Field* field;
		void* data;

		template <typename T>
		T& GetData() const
		{
			return *reinterpret_cast<T*>(data);
		}

		template <typename TClass>
		bool IsTypeClass() const
		{
			return field->GetType()->IsTypeClass<TClass>();
		}
	};

	auto GetFields()
	{
		auto read_field = [this](const il2cpp::Field& field) {
			return _GetFieldResult{
				.field = &field,
				.data = reinterpret_cast<uint8_t*>(this) + field.GetOffset(),
			};
		};

		return klass->GetFields()
			| std::views::filter(&il2cpp::Field::IsInstance)
			| std::views::transform(read_field);
	}

	template <typename TClass>
	auto GetFields()
	{
		return GetFields()
			| std::views::filter(&System::Object::_GetFieldResult::IsTypeClass<std::remove_pointer_t<TClass>>)
			| std::views::transform(&System::Object::_GetFieldResult::GetData<TClass>);
	}

private:

	const il2cpp::Class* klass;
	struct Il2CppMonitorData* monitor;
};

} // namespace System
