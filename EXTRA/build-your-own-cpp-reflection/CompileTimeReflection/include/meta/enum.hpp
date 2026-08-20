#pragma once

#include <functional>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <meta/core.hpp>

#define META_ENUM_VALUE(Enum, Enumerator) \
    ::meta::enum_value_desc<#Enumerator, Enum::Enumerator> {}

#define META_ENUM_VALUE_AS(Name, Value) \
    ::meta::enum_value_desc<Name, Value> {}

namespace meta
{

    /** One explicitly registered name/value pair from an enum. */
    template <fixed_string Name, auto Value>
    struct enum_value_desc
    {
        static_assert(std::is_enum_v<decltype(Value)>,
                      "enum_value_desc requires an enum value");

        static constexpr auto name  = Name;
        static constexpr auto value = Value;

        using enum_type       = decltype(Value);
        using underlying_type = std::underlying_type_t<enum_type>;
    };

    namespace detail
    {

        template <typename Tuple, std::size_t I = 0, std::size_t J = 1>
        consteval bool enum_names_are_unique() {
            constexpr auto count = std::tuple_size_v<Tuple>;

            if constexpr (I >= count) {
                return true;
            }
            else if constexpr (J >= count) {
                return enum_names_are_unique<Tuple, I + 1, I + 2>();
            }
            else {
                using left  = std::tuple_element_t<I, Tuple>;
                using right = std::tuple_element_t<J, Tuple>;
                return left::name.view() != right::name.view() &&
                       enum_names_are_unique<Tuple, I, J + 1>();
            }
        }

    } // namespace detail

    /**
     * Validate one enum registry.
     *
     * Every item must belong to E and every reflected name must be unique.
     * Duplicate underlying values remain legal because C++ enum aliases are a
     * real use case; enum_name returns the first registered alias.
     */
    template <typename E>
        requires reflected_enum<E>
    consteval bool enum_metadata_valid() {
        using type        = std::remove_cvref_t<E>;
        using values_type = std::remove_cvref_t<
            decltype(enum_reflect<type>::values)>;

        constexpr bool correct_enum_type = std::apply(
            []<typename... Items>(Items...) {
                return (true && ... &&
                        std::is_same_v<typename Items::enum_type, type>);
            },
            enum_reflect<type>::values);

        return correct_enum_type &&
               detail::enum_names_are_unique<values_type>();
    }

    /** Visit enum descriptors in explicit registration order. */
    template <typename E, typename Fn>
        requires reflected_enum<E>
    constexpr void for_each_enum(Fn&& function) {
        using type = std::remove_cvref_t<E>;
        static_assert(enum_metadata_valid<type>(),
                      "invalid enum reflection metadata");

        std::apply(
            [&]<typename... Items>(const Items&... items) {
                (std::invoke(function, items), ...);
            },
            enum_reflect<type>::values);
    }

    /** Convert an enum value to its first registered name. */
    template <typename E>
        requires reflected_enum<E>
    [[nodiscard]] constexpr std::optional<std::string_view> enum_name(E value) {
        using type = std::remove_cvref_t<E>;
        static_assert(enum_metadata_valid<type>(),
                      "invalid enum reflection metadata");

        std::optional<std::string_view> result;
        std::apply(
            [&](auto... item) {
                auto inspect = [&](auto descriptor) {
                    using item_type = decltype(descriptor);
                    if (!result && item_type::value == value) {
                        result = item_type::name.view();
                    }
                };
                (inspect(item), ...);
            },
            enum_reflect<type>::values);
        return result;
    }

    /** Convert a registered enum name back to its exact enum value. */
    template <typename E>
        requires reflected_enum<E>
    [[nodiscard]] constexpr std::optional<std::remove_cvref_t<E>>
    enum_cast(std::string_view name) {
        using type = std::remove_cvref_t<E>;
        static_assert(enum_metadata_valid<type>(),
                      "invalid enum reflection metadata");

        std::optional<type> result;
        std::apply(
            [&](auto... item) {
                auto inspect = [&](auto descriptor) {
                    using item_type = decltype(descriptor);
                    if (!result && item_type::name.view() == name) {
                        result = item_type::value;
                    }
                };
                (inspect(item), ...);
            },
            enum_reflect<type>::values);
        return result;
    }

} // namespace meta
