#pragma once

namespace meta
{
    template<typename Return, typename Object, bool HasConst, typename... Args>
    struct member_function_trait
    {
        template<typename Return, typename Object, typename... Args>
        explicit constexpr member_function_trait(Return(Object::*)(Args...)const) { }

        template<typename Return, typename Object, typename... Args>
        explicit constexpr member_function_trait(Return(Object::*)(Args...)) { }

        using return_type = Return;
        using object_type = Object;
        using args_tuple = std::tuple<Args...>;

        static constexpr bool has_args = sizeof...(Args) > 0;
        static constexpr bool has_const = HasConst;
    };

    template<typename Return, typename Object, typename... Args>
    member_function_trait(Return(Object::*)(Args...)const) -> member_function_trait<Return, Object, true, Args...>;

    template<typename Return, typename Object, typename... Args>
    member_function_trait(Return(Object::*)(Args...)) -> member_function_trait<Return, Object, false, Args...>;

    template<auto MemFuncPtr>
    struct member_function
    {
        using type = decltype(MemFuncPtr);
        using trait = decltype(member_function_trait{ MemFuncPtr });
        using return_type = trait::template return_type;
        using object_type = trait::template object_type;
        using args_tuple = trait::template args_tuple;
        template<size_t Index>
        using nth_arg = std::tuple_element_t<Index, args_tuple>;

        static constexpr bool has_args = trait::template has_args;
        static constexpr bool has_const = trait::template has_const;
    };
}