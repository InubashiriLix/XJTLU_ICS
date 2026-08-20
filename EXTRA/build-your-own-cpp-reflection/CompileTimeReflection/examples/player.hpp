#pragma once

#include <algorithm>
#include <string>
#include <tuple>

#include <meta/reflection.hpp>

namespace example
{

    struct Stats
    {
        int   level;
        float accuracy;
    };

    // Sparse values and an alias make the enum example exercise behavior that
    // a simple zero-based array implementation would get wrong.
    enum class PlayerState : int
    {
        idle        = -1,
        moving      = 4,
        dead        = 9,
        knocked_out = dead,
    };

    // The example type deliberately mixes fields, an ordinary method, two
    // overloads, nested custom metadata and an enum so each path is shown.
    struct Player
    {
        int         hp;
        float       speed;
        std::string name;
        Stats       stats;
        PlayerState state;

        void heal(int amount) {
            hp = std::clamp(hp + amount, 0, 100);
        }

        void set_speed(float value) {
            speed = value;
        }

        void set_speed(int value) {
            speed = static_cast<float>(value);
        }

        [[nodiscard]] int health() const noexcept {
            return hp;
        }
    };

    inline int total_score(const Player& player, int bonus) noexcept {
        return player.hp + bonus;
    }

    inline std::string describe(const Player& player) {
        return player.name + ":" + std::to_string(player.hp);
    }

    inline int adjust_score(int score, int bonus) noexcept {
        return score + bonus;
    }

    inline int adjust_score(int score, float multiplier) noexcept {
        return static_cast<int>(static_cast<float>(score) * multiplier);
    }

    struct game_api;

} // namespace example

template <>
struct meta::reflect<example::Stats>
{
    static constexpr auto name = meta::fixed_string {"Stats"};

    static constexpr auto fields = std::tuple {
        META_FIELD(example::Stats, level),
        META_FIELD(example::Stats, accuracy),
    };
};

// Registering fields by hand would look like this:
//
//   meta::field_desc<"hp", &example::Player::hp> {}
//
// META_FIELD only removes that repetition. It does not change the underlying
// descriptor type or perform compiler-provided reflection.
template <>
struct meta::reflect<example::Player>
{
    static constexpr auto name = meta::fixed_string {"Player"};

    static constexpr auto fields = std::tuple {
        META_FIELD(example::Player, hp),
        META_FIELD(example::Player, speed),
        META_FIELD(example::Player, name),
        META_FIELD(example::Player, stats),
        META_FIELD(example::Player, state),
    };

    // Methods live in a separate tuple. Putting META_METHOD into `fields`
    // would make field iteration try to access a nonexistent `member` value.
    static constexpr auto methods = std::tuple {
        META_METHOD(example::Player, heal),
        META_METHOD_SIG(example::Player,
                        set_speed,
                        void (example::Player::*)(float)),
        META_METHOD_SIG(example::Player,
                        set_speed,
                        void (example::Player::*)(int)),
        META_METHOD(example::Player, health),
    };
};

template <>
struct meta::enum_reflect<example::PlayerState>
{
    static constexpr auto name = meta::fixed_string {"PlayerState"};

    static constexpr auto values = std::tuple {
        META_ENUM_VALUE(example::PlayerState, idle),
        META_ENUM_VALUE(example::PlayerState, moving),
        META_ENUM_VALUE(example::PlayerState, dead),
        META_ENUM_VALUE(example::PlayerState, knocked_out),
    };
};

// A tag gives unrelated free functions an explicit compile-time owner without
// introducing a global registry shared by every module.
template <>
struct meta::function_registry<example::game_api>
{
    static constexpr auto functions = std::tuple {
        // META_FUNCTION_AS keeps the public reflected name short even though
        // the actual C++ function is namespace-qualified at this point.
        META_FUNCTION_AS("total_score", example::total_score),
        META_FUNCTION_AS("describe", example::describe),
        META_FUNCTION_SIG_AS("adjust_score",
                             example::adjust_score,
                             int (*)(int, int) noexcept),
        META_FUNCTION_SIG_AS("adjust_score",
                             example::adjust_score,
                             int (*)(int, float) noexcept),
    };
};
