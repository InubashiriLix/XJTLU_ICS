#pragma once

#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include <meta/core.hpp>

// Registration macros only build descriptors. They do not discover members by
// themselves; the user still decides which fields are reflected.
#define META_FIELD(Type, Member) \
    ::meta::field_desc<#Member, &Type::Member> {}

namespace meta
{

    template <typename T>
    struct member_object_pointer_traits;

    template <typename Owner, typename Value>
    struct member_object_pointer_traits<Value Owner::*>
    {
        using owner_type = Owner;
        using value_type = Value;
    };

    /** Metadata for one non-static data member. */
    template <fixed_string Name, auto Member>
    struct field_desc
    {
        static_assert(std::is_member_object_pointer_v<decltype(Member)>,
                      "field_desc requires a non-static data member pointer");

        static constexpr auto name   = Name;
        static constexpr auto member = Member;

        using pointer_type = decltype(Member);
        using owner_type =
            typename member_object_pointer_traits<pointer_type>::owner_type;
        using value_type =
            typename member_object_pointer_traits<pointer_type>::value_type;
    };

    /** Visit every registered field as `(name, value)` without type erasure. */
    template <typename T, typename Fn>
    constexpr void for_each_field(T&& object, Fn&& function) {
        using type = std::remove_cvref_t<T>;
        std::apply(
            [&](auto... field) {
                (std::invoke(function,
                             decltype(field)::name.view(),
                             std::invoke(decltype(field)::member,
                                         std::forward<T>(object))),
                 ...);
            },
            reflect<type>::fields);
    }

    /** Find a field by compile-time name and preserve its exact reference type. */
    template <fixed_string Name, std::size_t I = 0, typename T>
    constexpr decltype(auto) get_field(T&& object) {
        using type        = std::remove_cvref_t<T>;
        using fields_type = std::remove_cvref_t<decltype(reflect<type>::fields)>;

        constexpr auto count = std::tuple_size_v<fields_type>;
        if constexpr (I == count) {
            static_assert(I != count, "reflection field not found");
        }
        else {
            using field = std::tuple_element_t<I, fields_type>;
            if constexpr (field::name.view() == Name.view()) {
                return std::invoke(field::member, std::forward<T>(object));
            }
            else {
                return get_field<Name, I + 1>(std::forward<T>(object));
            }
        }
    }

    // Backward-compatible short name used by the original example.
    template <fixed_string Name, typename T>
    constexpr decltype(auto) get(T&& object) {
        return get_field<Name>(std::forward<T>(object));
    }

    /** Check field existence from type metadata; no real object is required. */
    template <fixed_string Name, typename T>
    consteval bool has_field() {
        using type = std::remove_cvref_t<T>;
        return std::apply(
            []<typename... Fields>(Fields...) {
                return (false || ... || (Fields::name.view() == Name.view()));
            },
            reflect<type>::fields);
    }

    // Object-taking overload kept for exploratory code written against the
    // original API. The object is intentionally unused.
    template <fixed_string Name, typename T>
    constexpr bool has_field(T&&) {
        return has_field<Name, std::remove_cvref_t<T>>();
    }

    namespace detail
    {

        template <typename T, typename Visitor>
        constexpr void for_each_field_recursive_impl(T&          object,
                                                     Visitor&    visitor,
                                                     std::size_t depth) {
            using type = std::remove_cvref_t<T>;

            std::apply(
                [&]<typename... Fields>(Fields... fields) {
                    auto visit = [&](auto field) {
                        using field_type = decltype(field);
                        auto&& value     = std::invoke(field_type::member, object);

                        // Pre-order traversal: report the composite field before
                        // descending into the custom object stored inside it.
                        std::invoke(visitor, field, value, depth);

                        using value_type = std::remove_cvref_t<decltype(value)>;
                        if constexpr (is_reflected_object_v<value_type>) {
                            for_each_field_recursive_impl(value,
                                                          visitor,
                                                          depth + 1);
                        }
                    };

                    (visit(fields), ...);
                },
                reflect<type>::fields);
        }

    } // namespace detail

    /**
     * Traverse registered custom objects in pre-order.
     *
     * The callback receives `(field_descriptor, value, depth)` for every field.
     * Only explicitly reflected class/struct values recurse. Enums, pointers and
     * standard containers remain leaves so ownership is never guessed.
     */
    template <typename T, typename Visitor>
        requires reflected_object<T>
    constexpr void for_each_field_recursive(T& object, Visitor&& visitor) {
        auto&& visitor_reference = visitor;
        detail::for_each_field_recursive_impl(object,
                                              visitor_reference,
                                              std::size_t {0});
    }

} // namespace meta
