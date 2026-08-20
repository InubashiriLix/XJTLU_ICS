#include <cassert>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "player.hpp"

namespace test_types
{

    struct Unregistered
    {};

    enum class UnregisteredEnum
    {
        value,
    };

    struct ContainerHolder
    {
        std::vector<example::Stats> values;
        example::Stats*             pointer;
    };

} // namespace test_types

template <>
struct meta::reflect<test_types::ContainerHolder>
{
    static constexpr auto name = meta::fixed_string {"ContainerHolder"};

    static constexpr auto fields = std::tuple {
        META_FIELD(test_types::ContainerHolder, values),
        META_FIELD(test_types::ContainerHolder, pointer),
    };
};

namespace
{

    using example::Player;
    using example::PlayerState;
    using example::Stats;

    using heal_signature         = void (Player::*)(int);
    using speed_float_signature  = void (Player::*)(float);
    using speed_int_signature    = void (Player::*)(int);
    using health_signature       = int (Player::*)() const noexcept;
    using score_signature        = int (*)(const Player&, int) noexcept;
    using describe_signature     = std::string (*)(const Player&);
    using adjust_int_signature   = int (*)(int, int) noexcept;
    using adjust_float_signature = int (*)(int, float) noexcept;

    static_assert(meta::has_field<"hp", Player>());
    static_assert(meta::has_field<"speed", Player>());
    static_assert(!meta::has_field<"level", Player>());

    static_assert(meta::is_reflected_object_v<Player>);
    static_assert(meta::is_reflected_object_v<Stats>);
    static_assert(meta::is_enum_reflected_v<PlayerState>);
    static_assert(meta::is_reflected_v<Player>);
    static_assert(meta::is_reflected_v<PlayerState>);
    static_assert(!meta::is_reflected_v<int>);
    static_assert(!meta::is_reflected_v<test_types::Unregistered>);
    static_assert(!meta::is_reflected_v<test_types::UnregisteredEnum>);
    static_assert(meta::type_name<Player>() == "Player");
    static_assert(meta::type_name<Stats>() == "Stats");
    static_assert(meta::type_name<PlayerState>() == "PlayerState");

    using player_fields = std::remove_cvref_t<
        decltype(meta::reflect<Player>::fields)>;
    using stats_field = std::tuple_element_t<3, player_fields>;
    static_assert(std::is_same_v<stats_field::owner_type, Player>);
    static_assert(std::is_same_v<stats_field::value_type, Stats>);
    static_assert(std::is_same_v<
                  stats_field::pointer_type,
                  Stats Player::*>);

    static_assert(meta::has_method<"heal", heal_signature, Player>());
    static_assert(meta::has_method<"set_speed", speed_float_signature, Player>());
    static_assert(meta::has_method<"set_speed", speed_int_signature, Player>());
    static_assert(!meta::has_method<"missing", heal_signature, Player>());

    static_assert(meta::has_function<
                  example::game_api,
                  "total_score",
                  score_signature>());
    static_assert(meta::has_function<
                  example::game_api,
                  "describe",
                  describe_signature>());
    static_assert(meta::has_function<
                  example::game_api,
                  "adjust_score",
                  adjust_int_signature>());
    static_assert(meta::has_function<
                  example::game_api,
                  "adjust_score",
                  adjust_float_signature>());

    using health_descriptor = std::remove_cvref_t<decltype(meta::get_method<"health", health_signature, Player>())>;
    using health_traits     = health_descriptor::traits;

    static_assert(health_traits::is_member);
    static_assert(health_traits::is_const);
    static_assert(health_traits::is_noexcept);
    static_assert(health_traits::arity == 0);
    static_assert(std::is_same_v<health_traits::class_type, Player>);
    static_assert(std::is_same_v<health_traits::return_type, int>);

    using score_descriptor = std::remove_cvref_t<decltype(meta::get_function<
                                                          example::game_api,
                                                          "total_score",
                                                          score_signature>())>;
    using score_traits     = score_descriptor::traits;

    static_assert(!score_traits::is_member);
    static_assert(!score_traits::is_const);
    static_assert(score_traits::is_noexcept);
    static_assert(score_traits::arity == 2);
    static_assert(std::is_same_v<
                  score_traits::argument_type<0>,
                  const Player&>);
    static_assert(std::is_same_v<score_traits::argument_type<1>, int>);

    void test_fields() {
        Player player {
            20,
            1.0F,
            "Ada",
            Stats {1, 0.5F},
            PlayerState::idle,
        };

        static_assert(std::is_same_v<
                      decltype(meta::get_field<"hp">(player)),
                      int&>);

        const Player& const_player = player;
        static_assert(std::is_same_v<
                      decltype(meta::get_field<"hp">(const_player)),
                      const int&>);

        meta::get<"hp">(player) = 42;
        assert(player.hp == 42);
        assert(meta::has_field<"name">(player));

        std::size_t count = 0;
        std::string names;
        meta::for_each_field(
            player,
            [&](std::string_view name, const auto&) {
                ++count;
                names.append(name);
                names.push_back(' ');
            });

        assert(count == 5);
        assert(names == "hp speed name stats state ");
    }

    void test_recursive_fields() {
        Player player {
            50,
            1.5F,
            "Ada",
            Stats {7, 0.8F},
            PlayerState::moving,
        };

        std::string traversal;
        meta::for_each_field_recursive(
            player,
            [&]<typename Field>(Field, auto& value, std::size_t depth) {
                traversal.append(depth, '>');
                traversal.append(Field::name.view());
                traversal.push_back(' ');

                if constexpr (std::is_same_v<typename Field::owner_type,
                                             Stats> &&
                              std::is_same_v<typename Field::value_type, int>) {
                    value = 8;
                }
            });

        assert(traversal ==
               "hp speed name stats >level >accuracy state ");
        assert(player.stats.level == 8);

        const Player& const_player      = player;
        std::size_t   const_field_count = 0;
        meta::for_each_field_recursive(
            const_player,
            [&]<typename Field>(Field, auto& value, std::size_t) {
                static_assert(std::is_const_v<
                              std::remove_reference_t<decltype(value)>>);
                ++const_field_count;
            });
        assert(const_field_count == 7);

        Stats                       stats {1, 0.5F};
        test_types::ContainerHolder holder {{stats}, &stats};
        std::size_t                 holder_field_count = 0;
        meta::for_each_field_recursive(
            holder,
            [&](auto, auto&, std::size_t depth) {
                assert(depth == 0);
                ++holder_field_count;
            });
        assert(holder_field_count == 2);
    }

    void test_enums() {
        static_assert(meta::enum_metadata_valid<PlayerState>());

        assert(meta::enum_name(PlayerState::idle) == "idle");
        assert(meta::enum_name(PlayerState::moving) == "moving");

        // `dead` and `knocked_out` have the same underlying value. The first
        // registered name wins for value-to-name conversion.
        assert(meta::enum_name(PlayerState::knocked_out) == "dead");
        assert(!meta::enum_name(static_cast<PlayerState>(123)));

        assert(meta::enum_cast<PlayerState>("idle") == PlayerState::idle);
        assert(meta::enum_cast<PlayerState>("knocked_out") ==
               PlayerState::knocked_out);
        assert(!meta::enum_cast<PlayerState>("missing"));

        std::string names;
        meta::for_each_enum<PlayerState>(
            [&]<typename Item>(Item) {
                names.append(Item::name.view());
                names.push_back(' ');
            });
        assert(names == "idle moving dead knocked_out ");
    }

    void test_methods() {
        Player player {
            80,
            1.0F,
            "Ada",
            Stats {1, 0.5F},
            PlayerState::idle,
        };

        meta::invoke_method<"heal", heal_signature>(player, 15);
        assert(player.hp == 95);

        meta::invoke_method<"heal", heal_signature>(player, 20);
        assert(player.hp == 100);

        Player* player_pointer = &player;
        meta::invoke_method<"heal", heal_signature>(player_pointer, -10);
        assert(player.hp == 90);

        meta::invoke_method<"set_speed", speed_float_signature>(
            player,
            2.5F);
        assert(player.speed == 2.5F);

        meta::invoke_method<"set_speed", speed_int_signature>(player, 4);
        assert(player.speed == 4.0F);

        const Player& const_player = player;
        assert(meta::invoke_method<"health", health_signature>(const_player) ==
               90);

        std::size_t method_count    = 0;
        std::size_t set_speed_count = 0;
        meta::for_each_method<Player>(
            [&]<typename Descriptor>(Descriptor) {
                ++method_count;
                if (Descriptor::name.view() == "set_speed") {
                    ++set_speed_count;
                }
            });

        assert(method_count == 4);
        assert(set_speed_count == 2);
    }

    void test_free_functions() {
        const Player player {
            90,
            1.0F,
            "Ada",
            Stats {1, 0.5F},
            PlayerState::idle,
        };

        const int score = meta::invoke_function<
            example::game_api,
            "total_score",
            score_signature>(player, 7);
        assert(score == 97);

        const auto description = meta::invoke_function<
            example::game_api,
            "describe",
            describe_signature>(player);
        assert(description == "Ada:90");

        assert((meta::invoke_function<
                    example::game_api,
                    "adjust_score",
                    adjust_int_signature>(90, 7) == 97));
        assert((meta::invoke_function<
                    example::game_api,
                    "adjust_score",
                    adjust_float_signature>(90, 1.5F) == 135));

        std::size_t function_count = 0;
        meta::for_each_function<example::game_api>(
            [&](const auto&) {
                ++function_count;
            });
        assert(function_count == 4);
    }

} // namespace

int main() {
    test_fields();
    test_recursive_fields();
    test_enums();
    test_methods();
    test_free_functions();
}
