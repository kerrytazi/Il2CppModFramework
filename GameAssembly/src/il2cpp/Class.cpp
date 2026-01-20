#include "pch.hpp"

#include "il2cpp/Class.hpp"

#include "il2cpp/Method.hpp"
#include "il2cpp/Field.hpp"
#include "il2cpp/Property.hpp"
#include "il2cpp/Event.hpp"

#include "il2cpp_data.hpp"

#if !defined(UC_UNITY_VERSION_NUM)
#error UC_UNITY_VERSION_NUM is not defined
#endif

std::string_view il2cpp::Class::GetName() const
{
	return name;
}

std::string_view il2cpp::Class::GetNamespace() const
{
	return namespaze;
}

std::span<const il2cpp::Method*> il2cpp::Class::GetMethods() const
{
	return std::span(methods, method_count);
}

std::span<const il2cpp::Field> il2cpp::Class::GetFields() const
{
	return std::span(fields, field_count);
}

std::span<const il2cpp::Property> il2cpp::Class::GetProperties() const
{
	return std::span(properties, property_count);
}

std::span<const il2cpp::Event> il2cpp::Class::GetEvents() const
{
	return std::span(events, event_count);
}

bool il2cpp::Class::IsInitialized() const
{
	return initialized;
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

const il2cpp::Method* il2cpp::Class::FindMethod(
	std::string_view method_name,
	std::string_view ret_type,
	std::initializer_list<std::string_view> param_types,
	std::optional<bool> is_static /*= std::nullopt*/) const
{
	for (auto method : GetMethods())
	{
		if (method->GetParametersCount() != param_types.size())
			continue;

		if (is_static.has_value())
			if (is_static != method->IsStatic())
				continue;

		if (method->GetName() != method_name)
			continue;

		if (method->GetReturnType()->GetName() != ret_type)
			continue;

		auto params_view = method->GetParameterTypesView();

		auto it1 = param_types.begin();
		auto it2 = params_view.begin();
		for (; it1 != param_types.end(); ++it1, ++it2)
		{
			if ((*it2)->GetName() != *it1)
				break;
		}

		if (it1 != param_types.end())
			continue;

		return method;
	}

	return nullptr;
}
