#include "pch.hpp"

#include "il2cpp/Class.hpp"

#include "il2cpp/Method.hpp"
#include "il2cpp/Field.hpp"
#include "il2cpp/Property.hpp"
#include "il2cpp/Event.hpp"

#include "il2cpp_data.hpp"

#if !defined(ICMF_UNITY_VERSION_NUM)
#error ICMF_UNITY_VERSION_NUM is not defined
#endif

const il2cpp::Type* il2cpp::Class::GetType() const
{
	return &byval_arg;
}

std::string_view il2cpp::Class::GetName() const
{
	return name;
}

std::string_view il2cpp::Class::GetNamespace() const
{
	return namespaze;
}

bool il2cpp::Class::IsValueType() const
{
	return byval_arg.IsValueType();
}

std::string il2cpp::Class::BuildFullName() const
{
	return _BuildFullName();
}

void il2cpp::Class::BuildFullName(std::string& cache) const
{
	cache.clear();

	auto name = GetName();
	cache.insert(cache.begin(), name.begin(), name.end());

	auto k = this;

	while (k->GetDeclaringClass())
	{
		k = k->GetDeclaringClass();
		auto name = k->GetName();
		cache.insert(cache.begin(), '.');
		cache.insert(cache.begin(), name.begin(), name.end());
	}

	if (!k->GetNamespace().empty())
	{
		auto name = k->GetNamespace();
		cache.insert(cache.begin(), '.');
		cache.insert(cache.begin(), name.begin(), name.end());
	}
}

static std::string g_build_full_name_cache;
const std::string& il2cpp::Class::_BuildFullName() const
{
	BuildFullName(g_build_full_name_cache);
	return g_build_full_name_cache;
}

void* il2cpp::Class::_GetStaticFields() const
{
	return static_fields;
}

const il2cpp::Class* il2cpp::Class::GetDeclaringClass() const
{
	return declaringType;
}

const il2cpp::Class* il2cpp::Class::GetElementClass() const
{
	return element_class;
}

const il2cpp::Method* il2cpp::Class::GetVirtualMethod(const Method* method) const
{
	assert(IsCastable(method->GetClass()));
	auto slot = method->GetVirtualMethodSlot();
	assert(slot < vtable_count);
	return vtable[slot].method;
}

const il2cpp::Class* il2cpp::Class::GetBase() const
{
	return parent;
}

bool il2cpp::Class::IsBaseOf(const Class* _derived) const
{
	if (_derived == nullptr)
		return false;

	if (this == _derived)
		return true;

	return IsBaseOf(_derived->GetBase());
}

bool il2cpp::Class::IsCastable(const Class* other) const
{
	return IsBaseOf(other) || other->IsBaseOf(this);
}

const il2cpp::Field* il2cpp::Class::FindField(std::string_view field_name) const
{
	for (auto field : GetFields())
	{
		if (field->IsStatic())
			continue;

		if (field->GetName() == field_name)
			return field;
	}

	return nullptr;
}

const il2cpp::Field* il2cpp::Class::FindFieldRecursive(std::string_view field_name) const
{
	auto k = this;

	while (k)
	{
		if (auto f = k->FindField(field_name))
			return f;

		k = k->GetBase();
	}

	return nullptr;
}

const il2cpp::Field* il2cpp::Class::FindStaticField(std::string_view field_name) const
{
	for (auto field : GetFields())
	{
		if (!field->IsStatic())
			continue;

		assert("Tried to find static field but found static literal" && (!field->IsLiteral() || field->GetName() != field_name));
		assert("Tried to find static field but found static thread local" && (!field->IsThreadLocal() || field->GetName() != field_name));

		if (field->GetName() == field_name)
			return field;
	}

	return nullptr;
}

const il2cpp::Field* il2cpp::Class::FindStaticFieldRecursive(std::string_view field_name) const
{
	auto k = this;

	while (k)
	{
		if (auto f = k->FindStaticField(field_name))
			return f;

		k = k->GetBase();
	}

	return nullptr;
}

const il2cpp::Field* il2cpp::Class::FindStaticLiteralField(std::string_view field_name) const
{
	for (const auto& field : GetFields())
	{
		if (!field->IsStatic())
			continue;

		assert("Tried to find static literal field but found static thread local" && (!field->IsThreadLocal() || field->GetName() != field_name));

		if (field->GetName() == field_name)
			return field;
	}

	return nullptr;
}

const il2cpp::Field* il2cpp::Class::FindStaticLiteralFieldRecursive(std::string_view field_name) const
{
	auto k = this;

	while (k)
	{
		if (auto f = k->FindStaticLiteralField(field_name))
			return f;

		k = k->GetBase();
	}

	return nullptr;
}

const il2cpp::Field* il2cpp::Class::FindStaticThreadLocalField(std::string_view field_name) const
{
	for (const auto& field : GetFields())
	{
		if (!field->IsStatic())
			continue;

		assert("Tried to find static thread local field but found static literal" && (!field->IsLiteral() || field->GetName() != field_name));

		if (field->GetName() == field_name)
			return field;
	}

	return nullptr;
}

const il2cpp::Field* il2cpp::Class::FindStaticThreadLocalFieldRecursive(std::string_view field_name) const
{
	auto k = this;

	while (k)
	{
		if (auto f = k->FindStaticThreadLocalField(field_name))
			return f;

		k = k->GetBase();
	}

	return nullptr;
}

const il2cpp::Method* il2cpp::Class::FindMethod(std::string_view method_name, int param_count) const
{
	for (auto method : GetMethods())
	{
		if (method->GetParametersCount() != param_count)
			continue;

		if (method->GetName() != method_name)
			continue;

		return method;
	}

	return nullptr;
}

const il2cpp::Class* il2cpp::Class::Find(std::string_view namespaze, std::string_view class_name)
{
	assert(g_il2cpp_data.GameAssembly);

	auto to_find = CachedClassFullName{ namespaze, class_name };
	auto cit = std::ranges::lower_bound(g_il2cpp_data.cached_classes, to_find, std::less<>{});

	if (cit == g_il2cpp_data.cached_classes.end())
		return nullptr;

	if (*cit != to_find)
		return nullptr;

	return cit->klass;
}

void il2cpp::Class::_ForceInitFull() const
{
	if (initialized)
		return;

	(void)GetType()->ToClass();

	if (auto k = GetBase())
		k->_ForceInitFull();

	if (auto k = GetElementClass(); k && k != this)
		k->_ForceInitFull();

	_ForceInitMethods();
	_ForceInitFields();
	_ForceInitProperties();
	_ForceInitEvents();
}

void il2cpp::Class::_ForceInitMethods() const
{
	if (initialized || method_count == 0 || methods)
		return;

	void* it = nullptr;
	g_il2cpp_data.il2cpp_class_get_methods(const_cast<il2cpp::Class*>(this), &it);
}

void il2cpp::Class::_ForceInitFields() const
{
	if (initialized || field_count == 0 || fields)
		return;

	void* it = nullptr;
	g_il2cpp_data.il2cpp_class_get_fields(const_cast<il2cpp::Class*>(this), &it);
}

void il2cpp::Class::_ForceInitProperties() const
{
	if (initialized || property_count == 0 || properties)
		return;

	void* it = nullptr;
	g_il2cpp_data.il2cpp_class_get_properties(const_cast<il2cpp::Class*>(this), &it);
}

void il2cpp::Class::_ForceInitEvents() const
{
	if (initialized || event_count == 0 || events)
		return;

	void* it = nullptr;
	g_il2cpp_data.il2cpp_class_get_events(const_cast<il2cpp::Class*>(this), &it);
}

const il2cpp::Method* il2cpp::Class::_FindMethod(
	std::string_view method_name,
	std::optional<std::string_view> ret_type /*= std::nullopt*/,
	std::optional<std::initializer_list<std::string_view>> param_types /*= std::nullopt*/,
	std::optional<bool> is_static /*= std::nullopt*/) const
{
	for (auto method : GetMethods())
	{
		if (param_types.has_value())
			if (method->GetParametersCount() != param_types->size())
				continue;

		if (is_static.has_value())
			if (*is_static != method->IsStatic())
				continue;

		if (method->GetName() != method_name)
			continue;

		if (ret_type.has_value())
			if (method->GetReturnType()->_GetName() != *ret_type)
				continue;

		if (param_types.has_value())
		{
			auto params_view = method->GetParameterTypesView();

			auto it1 = param_types->begin();
			auto it2 = params_view.begin();
			for (; it1 != param_types->end(); ++it1, ++it2)
			{
				if ((*it2)->_GetName() != *it1)
					break;
			}

			if (it1 != param_types->end())
				continue;
		}

		return method;
	}

	return nullptr;
}

const il2cpp::Method* il2cpp::Class::_FindMethodRecursive(
	std::string_view method_name,
	std::optional<std::string_view> ret_type /*= std::nullopt*/,
	std::optional<std::initializer_list<std::string_view>> param_types /*= std::nullopt*/,
	std::optional<bool> is_static /*= std::nullopt*/) const
{
	auto k = this;

	while (k)
	{
		if (auto m = k->_FindMethod(method_name, ret_type, param_types, is_static))
			return m;

		k = k->GetBase();
	}

	return nullptr;
}
