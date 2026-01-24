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

template <typename... Args>
struct __FuncArgsHelper
{
private:
	template <typename... Ts>
	static std::tuple<Ts...> make_tuple_from_pack(Ts...);

	using all_types_tuple = decltype(make_tuple_from_pack<Args...>(std::declval<Args>()...));

	template <size_t... Is>
	static auto remove_last_impl(std::index_sequence<Is...>)
	{
		constexpr size_t N = sizeof...(Args);
		if constexpr (N > 0)
		{
			return std::tuple<
				typename std::tuple_element<
				Is,
				all_types_tuple
				>::type...
			>{};
		}
		else
		{
			return std::tuple<>{};
		}
	}

public:
	using last_type = typename std::tuple_element<
		sizeof...(Args) - 1,
		all_types_tuple
	>::type;

	using args_except_last_tuple = decltype(
		remove_last_impl(std::make_index_sequence<sizeof...(Args) - 1>{})
	);

	template <typename Tuple>
	struct function_ptr_from_tuple;

	template <typename... TupleArgs>
	struct function_ptr_from_tuple<std::tuple<TupleArgs...>>
	{
		using type = last_type(*)(TupleArgs...);
	};

	using function_pointer_type = typename function_ptr_from_tuple<args_except_last_tuple>::type;
	static constexpr size_t count = sizeof...(Args);
};

void* __CreateMethodInfoForFunc(const void* func, size_t parameters_count);
const il2cpp::Class* __FindFuncClass(size_t templates_count);

namespace System
{

template <typename... TArgsAndRet>
class Func : public System::MulticastDelegate
{
	using _Helper = __FuncArgsHelper<TArgsAndRet...>;

public:

	System::Void _ctor(System::Object* object, System::IntPtr method)
	{
		auto func = CallCached<decltype([]() {
			auto klass = il2cpp::FindClassOnce<Func<TArgsAndRet...>>::Find(); assert(klass);
			auto method = klass->FindMethod(".ctor", "System.Void", { "System.Object", "System.IntPtr" }, false); assert(method);
			auto method_ptr = method->template GetMethodPointer<System::Void(decltype(this), System::Object*, System::IntPtr)>(); assert(method_ptr);
			return method_ptr;
		}) > ();

		func(this, object, method);
	}

	static Func<TArgsAndRet...>* New(_Helper::function_pointer_type func)
	{
		auto method = __CreateMethodInfoForFunc(func, _Helper::count - 1);
		auto obj = (Func<TArgsAndRet...>*)il2cpp::object_new(il2cpp::FindClassOnce<Func<TArgsAndRet...>>::Find());
		obj->_ctor(nullptr, System::IntPtr{ (intptr_t)method });
		return obj;
	}
};

} // namespace System

template <typename... TArgsAndRet>
struct il2cpp::FindClassOnce<System::Func<TArgsAndRet...>>
{
	using _Helper = __FuncArgsHelper<TArgsAndRet...>;

	static const Class* Find()
	{
		auto gklass = CallCached<decltype([]() {
			auto klass = __FindFuncClass(_Helper::count);
			auto type_args = il2cpp::Array<System::Type*>::New(_Helper::count);

			[&]<size_t... I>(std::index_sequence<I...>) {
				((type_args->at(I) = il2cpp::FindClassOnce<std::remove_pointer_t<typename std::tuple_element<I, std::tuple<TArgsAndRet...>>::type>>::Find()->GetType()->ToSystemType()), ...);
			}(std::make_index_sequence<_Helper::count>());

			auto gklass = klass->GetType()->ToSystemType()->MakeGenericType(type_args)->ToIl2CppType()->ToClass();
			return gklass;
		})>();
		return gklass;
	}
};
