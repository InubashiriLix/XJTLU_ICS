#include <iostream>
#include <string_view>
#include <type_traits>

#include "player.hpp"

int main() {
    using example::Player;

    Player player {
        80,
        1.0F,
        "Ada",
        example::Stats {3, 0.75F},
        example::PlayerState::moving,
    };

    // &Player::hp has type `int Player::*`: a pointer to an int data member of
    // Player. It is not a normal int pointer or a byte offset. Use `.*` with an
    // object and `->*` with an object pointer.
    int Player::* hp_member    = &Player::hp;
    player.*hp_member          = 80;
    Player* player_pointer     = &player;
    player_pointer->*hp_member = 80;

    static_assert(meta::is_reflected_v<Player>);
    static_assert(meta::is_reflected_v<example::PlayerState>);
    std::cout << "type: " << meta::type_name<Player>() << '\n';

    // Recursive traversal reports every parent field before descending into a
    // reflected custom value. Enums remain leaves and use enum_name explicitly.
    std::cout << "fields:\n";
    meta::for_each_field_recursive(
        player,
        []<typename Field>(Field, const auto& value, std::size_t depth) {
            using value_type = std::remove_cvref_t<decltype(value)>;
            std::cout << std::string(depth * 2 + 2, ' ')
                      << Field::name.view() << " = ";

            if constexpr (meta::is_reflected_object_v<value_type>) {
                std::cout << '<' << meta::type_name<value_type>() << '>';
            }
            else if constexpr (meta::is_enum_reflected_v<value_type>) {
                std::cout << meta::enum_name(value).value_or("<unknown>");
            }
            else {
                std::cout << value;
            }
            std::cout << '\n';
        });

    // The reflected form performs the same member-pointer operation after a
    // compile-time name lookup and still returns int& here.
    meta::get_field<"hp">(player) = 85;

    // A non-overloaded method can be registered with META_METHOD. Invocation
    // still states the signature so lookup remains unambiguous.
    meta::invoke_method<"heal", void (Player::*)(int)>(player, 10);

    // set_speed is overloaded, therefore registration and lookup both use the
    // exact member-function-pointer type to select the float overload.
    meta::invoke_method<"set_speed", void (Player::*)(float)>(player, 2.5F);

    // const and noexcept belong to a member-function pointer's type.
    using health_signature = int (Player::*)() const noexcept;
    const int hp           = meta::invoke_method<"health", health_signature>(player);

    const int score = meta::invoke_function<
        example::game_api,
        "total_score",
        int (*)(const Player&, int) noexcept>(player, 5);

    // The generic lambda receives an empty descriptor object. Its template
    // parameter carries all metadata at compile time.
    std::cout << "methods:\n";
    meta::for_each_method<Player>([]<typename Descriptor>(Descriptor) {
        std::cout << "  " << Descriptor::name.view()
                  << " (arity=" << Descriptor::traits::arity << ")\n";
    });

    std::cout << "hp=" << hp
              << ", speed=" << player.speed
              << ", score=" << score << '\n';
}
