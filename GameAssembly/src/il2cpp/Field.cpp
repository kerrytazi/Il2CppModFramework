#include "pch.hpp"

#include "il2cpp/Field.hpp"
#include "il2cpp/Type.hpp"
#include "il2cpp/il2cpp.hpp"

bool il2cpp::Field::IsStatic() const
{
	return type->attrs & FIELD_ATTRIBUTE_STATIC;
}

bool il2cpp::Field::IsLiteral() const
{
	return type->attrs & FIELD_ATTRIBUTE_LITERAL;
}

bool il2cpp::Field::IsThreadLocal() const
{
	return offset == -1;
}

void il2cpp::Field::GetLiteral(void* out) const
{
	assert("out can't be null" && out);
	assert("field is not static" && IsStatic());
	assert("field is not literal" && IsLiteral());
	assert("field is thread local" && !IsThreadLocal());
	il2cpp::field_static_get_value(this, out);
}

void il2cpp::Field::GetThreadLocal(void* out) const
{
	assert("out can't be null" && out);
	assert("field is not static" && IsStatic());
	assert("field is literal" && !IsLiteral());
	assert("field is not thread local" && IsThreadLocal());
	il2cpp::field_static_get_value(this, out);
}
