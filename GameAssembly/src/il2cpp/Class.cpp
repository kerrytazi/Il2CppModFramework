#include "pch.hpp"

#include "il2cpp/Class.hpp"

#include "il2cpp/Method.hpp"
#include "il2cpp/Field.hpp"
#include "il2cpp/Property.hpp"
#include "il2cpp/Event.hpp"

#include "il2cpp_data.hpp"

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

const il2cpp::Class* il2cpp::Class::Find(std::string_view namespaze, std::string_view class_name)
{
	assert(g_il2cpp_data.GameAssembly);

	auto cit = std::ranges::lower_bound(g_il2cpp_data.cached_classes, CachedClassFullName{ namespaze, class_name }, std::less<>{});

	if (cit == g_il2cpp_data.cached_classes.end())
		return nullptr;

	return cit->klass;
}
