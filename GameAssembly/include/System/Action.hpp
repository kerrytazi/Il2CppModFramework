#pragma once

#include "System/MulticastDelegate.hpp"
#include "System/Type.hpp"

#include "common/CallCached.hpp"
#include "il2cpp/il2cpp.hpp"
#include "il2cpp/Class.hpp"
#include "il2cpp/Method.hpp"
#include "il2cpp/ClassFinder.hpp"
#include "il2cpp/Array.hpp"

#include <type_traits>
#include <tuple>
#include <utility>

void __RegisterTmpMethodInfo(System::Object* obj, void* method);
[[nodiscard]]
void* __CreateMethodInfoForAction(const void* func, size_t parameters_count);
[[nodiscard]]
const il2cpp::Class* __FindActionClass(size_t templates_count);

namespace System
{

template <typename... TArgs>
class Action : public System::MulticastDelegate
{
public:

	System::Void _ctor(System::Object* object, System::IntPtr method)
	{
		auto func = CallCached<decltype([]() {
			auto klass = il2cpp::Find<Action<TArgs...>>(); assert(klass);
			auto method = klass->FindMethod(".ctor", "System.Void", { "System.Object", "System.IntPtr" }, false); assert(method);
			auto method_ptr = method->template GetMethodPointer<System::Void(decltype(this), System::Object*, System::IntPtr)>(); assert(method_ptr);
			return method_ptr;
		}) > ();

		func(this, object, method);
	}

	[[nodiscard]]
	static Action<TArgs...>* New(void(*func)(TArgs...))
	{
		auto method = __CreateMethodInfoForAction(func, sizeof...(TArgs));
		auto obj = (Action<TArgs...>*)il2cpp::object_new(il2cpp::Find<Action<TArgs...>>());
		obj->_ctor(nullptr, System::IntPtr{ (intptr_t)method });
		__RegisterTmpMethodInfo(method);
		return obj;
	}
};

} // namespace System

template <typename... TArgs>
struct il2cpp::FindClassOnce<System::Action<TArgs...>>
{
	static const Class* Find()
	{
		auto gklass = CallCached<decltype([]() {
			auto klass = __FindActionClass(sizeof...(TArgs));
			klass->_ForceInitFull();

			if constexpr (sizeof...(TArgs) > 0)
			{
				auto type_args = il2cpp::Array<System::Type*>::New(sizeof...(TArgs));

				[&]<size_t... I>(std::index_sequence<I...>) {
					((type_args->at(I) = il2cpp::Find<std::remove_pointer_t<typename std::tuple_element<I, std::tuple<TArgs...>>::type>>()->GetType()->ToSystemType()), ...);
				}(std::make_index_sequence<sizeof...(TArgs)>());

				auto gklass = klass->GetType()->ToReflectionType()->MakeGenericType(type_args)->ToIl2CppType()->ToClass();
				gklass->_ForceInitFull();
				return gklass;
			}
			else
			{
				return klass;
			}
		})>();
		return gklass;
	}
};
