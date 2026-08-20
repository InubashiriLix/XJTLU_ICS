#pragma once

#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <meta/core.hpp>

#define META_METHOD(Type, Method) \
    ::meta::callable_desc<#Method, &Type::Method> {}

#define META_METHOD_SIG(Type, Method, Signature) \
    ::meta::callable_desc<#Method, static_cast<Signature>(&Type::Method)> {}

#define META_FUNCTION(Function) \
    ::meta::callable_desc<#Function, &Function> {}

#define META_FUNCTION_AS(Name, Function) \
    ::meta::callable_desc<Name, &Function> {}

#define META_FUNCTION_SIG(Function, Signature) \
    ::meta::callable_desc<#Function, static_cast<Signature>(&Function)> {}

#define META_FUNCTION_SIG_AS(Name, Function, Signature) \
    ::meta::callable_desc<Name, static_cast<Signature>(&Function)> {}

namespace meta
{

    /**
     * Shared result storage for all callable_traits partial specializations.
     *
     * Class is void for a free/static function. Args contains only explicit
     * parameters; the implicit `this` object is not counted in arity.
     */
    template <typename Class,
              typename Return,
              bool IsMember,
              bool IsConst,
              bool IsNoexcept,
              typename... Args>
    struct callable_traits_base
    {
        using class_type  = Class;
        using return_type = Return;
        using args_type   = std::tuple<Args...>;

        template <std::size_t I>
        using argument_type = std::tuple_element_t<I, args_type>;

        static constexpr std::size_t arity       = sizeof...(Args);
        static constexpr bool        is_member   = IsMember;
        static constexpr bool        is_const    = IsConst;
        static constexpr bool        is_noexcept = IsNoexcept;
    };

    // An unsupported callable type intentionally produces an incomplete-type
    // compiler error. Add a partial specialization when supporting a new form.
    template <typename T>
    struct callable_traits;

    // Free functions (including static member functions after taking address).
    template <typename R, typename... Args>
    struct callable_traits<R (*)(Args...)>
        : callable_traits_base<void, R, false, false, false, Args...>
    {};

    template <typename R, typename... Args>
    struct callable_traits<R (*)(Args...) noexcept>
        : callable_traits_base<void, R, false, false, true, Args...>
    {};

    // Non-static member functions. const and noexcept are part of the pointer
    // type, therefore every supported combination needs its own specialization.
    template <typename C, typename R, typename... Args>
    struct callable_traits<R (C::*)(Args...)>
        : callable_traits_base<C, R, true, false, false, Args...>
    {};

    template <typename C, typename R, typename... Args>
    struct callable_traits<R (C::*)(Args...) const>
        : callable_traits_base<C, R, true, true, false, Args...>
    {};

    template <typename C, typename R, typename... Args>
    struct callable_traits<R (C::*)(Args...) noexcept>
        : callable_traits_base<C, R, true, false, true, Args...>
    {};

    template <typename C, typename R, typename... Args>
    struct callable_traits<R (C::*)(Args...) const noexcept>
        : callable_traits_base<C, R, true, true, true, Args...>
    {};

    /**
     * Compile-time description of a free function or member function.
     *
     * Pointer retains the exact callable type, including its owner, arguments,
     * const qualification and noexcept specification.
     */
    template <fixed_string Name, auto Pointer>
    struct callable_desc
    {
        static constexpr auto name    = Name;
        static constexpr auto pointer = Pointer;

        using pointer_type = decltype(Pointer);
        using traits       = callable_traits<pointer_type>;

        // std::invoke handles free functions, member functions, object
        // references and object pointers with one uniform expression.
        template <typename... Args>
        static constexpr decltype(auto) invoke(Args&&... args) noexcept(
            std::is_nothrow_invocable_v<pointer_type, Args...>) {
            return std::invoke(pointer, std::forward<Args>(args)...);
        }
    };

    /** Visit every descriptor in any heterogeneous callable registry tuple. */
    template <typename Tuple, typename Fn>
    constexpr void for_each_callable(const Tuple& registry, Fn&& function) {
        std::apply(
            [&]<typename... Items>(const Items&... items) {
                (std::invoke(function, items), ...);
            },
            registry);
    }

    /**
     * Test a callable key. The exact pointer signature is part of the key so
     * two overloads may safely share the same reflected name.
     */
    template <fixed_string Name, typename Signature, typename Tuple>
    consteval bool has_callable(const Tuple& registry) {
        return std::apply(
            []<typename... Items>(Items...) {
                return (false || ... ||
                        (Items::name.view() == Name.view() &&
                         std::is_same_v<typename Items::pointer_type, Signature>));
            },
            registry);
    }

    /**
     * Look up a descriptor in a heterogeneous tuple.
     *
     * A normal runtime loop could not return one of several unrelated element
     * types. The index recursion and if constexpr select exactly one type while
     * compiling; the final static_assert gives a useful missing-key error.
     */
    template <fixed_string Name,
              typename Signature,
              std::size_t I = 0,
              typename Tuple>
    constexpr decltype(auto) get_callable(Tuple&& registry) {
        using tuple_type     = std::remove_cvref_t<Tuple>;
        constexpr auto count = std::tuple_size_v<tuple_type>;

        if constexpr (I == count) {
            static_assert(I != count, "reflection callable not found");
        }
        else {
            using item = std::tuple_element_t<I, tuple_type>;
            if constexpr (item::name.view() == Name.view() &&
                          std::is_same_v<typename item::pointer_type, Signature>) {
                return std::get<I>(std::forward<Tuple>(registry));
            }
            else {
                return get_callable<Name, Signature, I + 1>(
                    std::forward<Tuple>(registry));
            }
        }
    }

    // Member-method convenience APIs select reflect<T>::methods before calling
    // the registry-independent algorithms above.
    template <typename T, typename Fn>
    constexpr void for_each_method(Fn&& function) {
        for_each_callable(reflect<std::remove_cvref_t<T>>::methods,
                          std::forward<Fn>(function));
    }

    template <fixed_string Name, typename Signature, typename T>
    consteval bool has_method() {
        return has_callable<Name, Signature>(
            reflect<std::remove_cvref_t<T>>::methods);
    }

    template <fixed_string Name, typename Signature, typename T>
    constexpr decltype(auto) get_method() {
        return get_callable<Name, Signature>(
            reflect<std::remove_cvref_t<T>>::methods);
    }

    template <fixed_string Name,
              typename Signature,
              typename T,
              typename... Args>
    constexpr decltype(auto) invoke_method(T&& object, Args&&... args) noexcept(
        std::is_nothrow_invocable_v<Signature, T, Args...>) {
        // std::invoke accepts either an object or an object pointer. Strip one
        // pointer layer only for registry selection; pass the original value to
        // the descriptor so the normal invocation rules still apply.
        using object_type    = std::remove_cvref_t<T>;
        using reflected_type = std::remove_cv_t<
            std::conditional_t<std::is_pointer_v<object_type>,
                               std::remove_pointer_t<object_type>,
                               object_type>>;

        auto&& descriptor = get_method<Name, Signature, reflected_type>();
        return descriptor.invoke(std::forward<T>(object),
                                 std::forward<Args>(args)...);
    }

    // Free-function convenience APIs select a tag-specific registry. The tag
    // acts as a compile-time namespace and has no runtime representation.
    template <typename Tag, typename Fn>
    constexpr void for_each_function(Fn&& function) {
        for_each_callable(function_registry<Tag>::functions,
                          std::forward<Fn>(function));
    }

    template <typename Tag, fixed_string Name, typename Signature>
    consteval bool has_function() {
        return has_callable<Name, Signature>(function_registry<Tag>::functions);
    }

    template <typename Tag, fixed_string Name, typename Signature>
    constexpr decltype(auto) get_function() {
        return get_callable<Name, Signature>(function_registry<Tag>::functions);
    }

    template <typename Tag,
              fixed_string Name,
              typename Signature,
              typename... Args>
    constexpr decltype(auto) invoke_function(Args&&... args) noexcept(
        std::is_nothrow_invocable_v<Signature, Args...>) {
        auto&& descriptor = get_function<Tag, Name, Signature>();
        return descriptor.invoke(std::forward<Args>(args)...);
    }

} // namespace meta
