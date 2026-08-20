#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace meta
{

    /**
     * A structural compile-time string that can be used as a C++20 NTTP.
     *
     * Notice the constructor parameter spelling: `const char (&text)[N]` is a
     * reference to an array of N const chars. `const char& text[N]` would mean
     * an invalid array of references and is not the same type.
     */
    template <std::size_t N>
    struct fixed_string
    {
        char data[N];

        constexpr fixed_string(const char (&text)[N]) noexcept {
            std::copy_n(text, N, data);
        }

        [[nodiscard]] constexpr std::string_view view() const noexcept {
            return {data, N - 1};
        }
    };

    // Class template argument deduction obtains N from the string literal, so
    // fixed_string{"name"} becomes fixed_string<5> automatically.
    template <std::size_t N>
    fixed_string(const char (&)[N]) -> fixed_string<N>;

    // Users specialize reflect<T> to register the name, fields and methods of
    // a custom class or struct. Old field/method APIs still work if `name` is
    // omitted, while the unified type-reflection APIs require full metadata.
    template <typename T>
    struct reflect;

    // Free functions have no natural owner type, so users group them under an
    // empty tag by specializing function_registry<Tag>.
    template <typename Tag>
    struct function_registry;

    // Enums use a separate customization point because their metadata consists
    // of named values rather than fields and methods.
    template <typename E>
    struct enum_reflect;

    template <typename T>
    using bare_type_t = std::remove_cvref_t<T>;

    template <typename T>
    concept reflected_object = requires {
        reflect<bare_type_t<T>>::name;
        reflect<bare_type_t<T>>::fields;
    };

    template <typename T>
    concept reflected_enum = std::is_enum_v<bare_type_t<T>> && requires {
        enum_reflect<bare_type_t<T>>::name;
        enum_reflect<bare_type_t<T>>::values;
    };

    template <typename T>
    inline constexpr bool is_reflected_object_v = reflected_object<T>;

    template <typename T>
    inline constexpr bool is_enum_reflected_v = reflected_enum<T>;

    template <typename T>
    inline constexpr bool is_reflected_v =
        is_reflected_object_v<T> || is_enum_reflected_v<T>;

    /** Return the explicitly registered name of a custom object or enum type. */
    template <typename T>
    consteval std::string_view type_name() {
        using type = bare_type_t<T>;

        if constexpr (is_reflected_object_v<type>) {
            return reflect<type>::name.view();
        }
        else if constexpr (is_enum_reflected_v<type>) {
            return enum_reflect<type>::name.view();
        }
        else {
            static_assert(is_reflected_v<type>,
                          "meta::type_name requires registered type metadata");
            return {};
        }
    }

} // namespace meta
