#pragma once
#undef min  //abolish vicious macros from <windows.h>, otherwise causing naming collision against STL
#undef max  //another tolerable solution appears like #define max_RESUME max #undef max ... #define max max_RESUME
namespace core
{
    namespace impl
    {
        template<typename Operation>
        struct static_invoke
        {
            template<typename ...Types>
            constexpr static auto by(Types ...args) -> std::decay_t<std::invoke_result_t<Operation,Types...>>
            {
                static_assert((... && std::is_standard_layout_v<Types>));
                //static_assert(std::is_standard_layout_v<std::common_type_t<Types...>>);
                return Operation{}(args...);
            }
        };
        template<typename BinaryOperation, typename, typename >
        struct bool_arithmetic;
        template<typename BinaryOperation, bool Left, bool Right>
        struct bool_arithmetic<BinaryOperation, std::bool_constant<Left>, std::bool_constant<Right>>
            :std::bool_constant<static_invoke<BinaryOperation>::by(Left, Right)> {};
        template<typename BinaryOperation, std::intmax_t RightFactor, typename T, T ...Vals>
        constexpr auto sequence_arithmetic(std::integer_sequence<T, Vals...> = {})
        {
            static_assert(std::is_invocable_r_v<T, BinaryOperation, const T&, const T&>);
            static_assert(std::is_convertible_v<decltype(RightFactor), T>);
            return std::integer_sequence < T, BinaryOperation{}(Vals, RightFactor)... > {};
        }
    }
    template<typename LeftBoolConst, typename RightBoolConst>
    struct bool_and :impl::bool_arithmetic<std::bit_and<bool>, LeftBoolConst, RightBoolConst> {};
    template<typename LeftBoolConst, typename RightBoolConst>
    struct bool_or :impl::bool_arithmetic<std::bit_or<bool>, LeftBoolConst, RightBoolConst> {};    
    template<typename LeftBoolConst, typename RightBoolConst>
    struct bool_xor :impl::bool_arithmetic<std::bit_xor<bool>, LeftBoolConst, RightBoolConst> {};
    namespace impl
    {
        template<typename T, typename U, typename... Types>
        struct within :bool_or<typename std::is_same<T, U>::type, typename within<T, Types...>::type> {};
        template<typename T, typename U>
        struct within<T, U> :std::is_same<T, U>::type {};
        template<typename T, T ...Vals>
        constexpr auto make_array(std::integer_sequence<T, Vals...> = {})
        {
            return std::array<T, sizeof...(Vals)>{ Vals... };
        }
        template<typename ...Types>
        constexpr auto make_array_any(Types... args)
        {
            return std::array<std::common_type_t<Types...>, sizeof...(args)>{
                static_cast<std::common_type_t<Types...>>(args)... };
        }
    }
    template<typename T, typename... Types>
    struct is_within :impl::within<T, Types...> 
    {
        static_assert(sizeof...(Types) > 1, "...Types has at least 2 elements");
    };
    template<typename T, typename... Types>
    constexpr bool is_within_v = is_within<T, Types...>::value;
    template<typename T>    //reference operation precede
    struct remove_cv_ref :std::remove_cv<std::remove_reference_t<T>> {};
    template<typename T>
    using remove_cv_ref_t = typename remove_cv_ref<T>::type;
    template<typename T>
    using atomic_value = decltype(std::declval<std::add_const_t<T>>().load());
    template<typename T>
    using future_value = decltype(std::declval<T>().get());
    template<typename T>
    struct is_future :core::is_within<core::remove_cv_ref_t<T>,
        std::future<core::future_value<T>>, std::shared_future<core::future_value<T>>> {};
    //template<typename T>
    //struct is_future<std::future<T>> :std::true_type {};
    //template<typename T>
    //struct is_future<std::shared_future<T>> :std::true_type {};
    template<typename T>
    struct is_atomic :std::is_same<core::remove_cv_ref_t<T>, std::atomic<core::atomic_value<T>>> {};
    using relative = std::int64_t;
    using absolute = std::uint64_t;
    using rel = relative;
    using abs = absolute;
    /**
    *  @return compile-time generated [Source,Dest] array, divided by Stride
    *  @note will be refactored to generic after template<auto> supported.
    */
    template<int Source, int Dest, int Stride = 1>
    constexpr auto range() 
    {
        static_assert(std::numeric_limits<int>::min() <= std::min<int>(Source, Dest));
        static_assert(std::numeric_limits<int>::max() >= std::max<int>(Source, Dest));
        static_assert(Source != Dest && Stride != 0 && ((Source < Dest) ^ (Stride < 0)));
        constexpr auto element_count = std::divides<int>{}(Dest - Source + Stride, Stride);
        return impl::make_array(core::impl::sequence_arithmetic<std::plus<int>, Source>(
            impl::sequence_arithmetic<std::multiplies<int>, Stride>(
                std::make_integer_sequence<int, element_count>{})));
    }
    namespace literals
    {
        constexpr size_t operator""_kilo(const size_t n) { return n * 1024; }
        constexpr size_t operator""_mega(const size_t n) { return n * 1024 * 1024; }
        constexpr size_t operator""_giga(const size_t n) { return n * 1024 * 1024 * 1024; }
    }
    inline auto directory_entry_count(const std::experimental::filesystem::path& directory)
    {   //non-recursive version, regardless of symbolic link
        const std::experimental::filesystem::directory_iterator iterator{ directory };
        return std::distance(begin(iterator), end(iterator));
    }
    inline auto thread_id = [hash = std::hash<std::thread::id>{}](std::optional<std::thread::id> id = std::nullopt)
    {
        return hash(id.value_or(std::this_thread::get_id()));
    };
    inline decltype(auto) repeat = [](auto count, auto&& callable, auto&& ...args)
    {
        static_assert(std::is_integral_v<decltype(count)>);
        if (std::invoke(callable, args...); --count > 0)
            return repeat(count, std::forward<decltype(callable)>(callable), std::forward<decltype(args)>(args)...);
        else
            return std::forward<decltype(callable)>(callable);
    };
    inline decltype(auto) repeat_each = [](auto&& callable, auto&& ...args)
    {
        std::invoke(callable, std::forward<decltype(args)>(args)...);
        return std::forward<decltype(callable)>(callable);
    };
    inline auto predicate = [](auto&& pred) constexpr ->bool
    {
        using pred_type = core::remove_cv_ref_t<decltype(pred)>;
        if constexpr(std::is_same_v<pred_type, bool>)
            return pred;
        else if constexpr(std::is_null_pointer_v<pred_type>)
            return false;
        else if constexpr(std::is_pointer_v<pred_type> && !std::is_member_pointer_v<pred_type>)
            return pred != nullptr;
        else if constexpr(core::is_atomic<pred_type>::value && std::is_lvalue_reference_v<decltype(pred)>)
            return pred.load(std::memory_order_acquire);
        else if constexpr(std::is_invocable_r_v<bool, pred_type>)
            return std::invoke(pred);
        else
            static_assert(false, "taste undesirable type");
    };
    inline decltype(auto) condition_loop = [](auto&& pred, auto&& callable)
    {
        static_assert(std::is_invocable_v<decltype(callable)>);
        //while (core::predicate(std::forward(pred)))
        while (core::predicate(pred))
            std::invoke(callable);
        return std::forward<decltype(callable)>(callable);
    };
}

