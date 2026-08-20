#pragma once

#include "reflection.h"
#include <string>

struct Player
{
    int         hp;
    float       speed;
    std::string name;
};

// okay now the register macro is basically highly binding with fucking structure definition
// so:
// WARNING:
// we se `REFLECTABLE_PLAYER_FIELD(X)` to register the fields
// However, in the class Player, we already have these fields
// This could be a bad idea to design the reflection like this
// because it increase the complexity, workload and mindload
// espcially when we maintain the structure with lots of fields
// in the fucking future
#define REFLECTABLE_PLAYER_FIELD(X) \
    X(int, hp)                      \
    X(float, speed)                 \
    X(std::string, name)

#define MAKE_PLAYER_FIELD(type, name) \
    FieldInfo {                       \
        #name, offsetof(Player, name) \
    }

/// we can use X macro techique to generate the fields
/// FieldInfo fields[] = {
//     REFLECTABLE_FIELD(MAKE_PLAYER_FIELD)};
