#include "pch.hpp"
#include "il2cpp/Method.hpp"

#include "il2cpp/Class.hpp"
#include "il2cpp/Type.hpp"

#include "common/Log.hpp"

#include "il2cpp_data.hpp"

#include "il2cpp/Image.hpp"
const il2cpp::Method* il2cpp::Method::Find(
	std::string_view namespaze,
	std::string_view class_name,
	std::string_view method_name,
	std::string_view ret_type,
	std::initializer_list<std::string_view> param_types,
	std::optional<bool> is_static /*= std::nullopt*/)
{
	if (auto klass = Class::Find(namespaze, class_name))
	{
		// FIXME!!!
		//if (!klass->IsInitialized())
		//{
		//	Log::Warn("Method::Find() failed: klass found but not initialized: " + std::string(class_name) + "::" + std::string(method_name) + ". This should never happen. Report to Developer.");
		//	return nullptr;
		//}

		for (auto method : klass->GetMethods())
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
	}

	return nullptr;
}
