# SHIT DESIGN

> okay this is a very BAD design
> cause it has cover typedefine by macro
> and in its design, we need to write each field in the struct again! in the macro.
> This increase the complexity, workload and mindload
> espcially when we maintain the structure with lots of fields
> [why it sucks](./inc/player.h)

比如我们希望最终做到：

```cpp
Player p;

set_field(&p, "hp", 100);
```

C++ 自己原本不知道 `"hp"` 对应什么，于是可以手动构造 metadata：

```cpp
struct FieldInfo {
    const char* name;
    size_t offset;
};

struct TypeInfo {
    const char* name;
    std::vector<FieldInfo> fields;
};
```

然后：

```cpp
struct Player {
    int hp;
    float speed;
};

TypeInfo PlayerInfo {
    "Player",
    {
        {"hp", offsetof(Player, hp)},
        {"speed", offsetof(Player, speed)}
    }
};
```

现在你其实就已经**手搓出一点反射了**：

```cpp
Player
 │
 ├─ hp
 │   ├─ name = "hp"
 │   └─ offset = 0
 │
 └─ speed
     ├─ name = "speed"
     └─ offset = 4
```

于是理论上：

```cpp
void* get_field(void* obj, const FieldInfo& field)
{
    return static_cast<char*>(obj) + field.offset;
}
```

就可以：

```cpp
auto hp = get_field(&p, PlayerInfo.fields[0]);

*static_cast<int*>(hp) = 100;
```

这已经跟 Python：

```cpp
setattr(p, "hp", 100)
```

有点神似了。

---

## 然后宏就是拿来消灭这些重复注册代码的

你显然不想写：

```cpp
{"hp", offsetof(Player, hp)}
{"speed", offsetof(Player, speed)}
{"name", offsetof(Player, name)}
...
```

于是可以：

```cpp
#define REFLECT_FIELD(Type, field) \
    FieldInfo{#field, offsetof(Type, field)}
```

然后：

```cpp
TypeInfo PlayerInfo {
    "Player",
    {
        REFLECT_FIELD(Player, hp),
        REFLECT_FIELD(Player, speed)
    }
};
```

这里有个很关键的宏技巧：

```cpp
#field
```

比如：

```cpp
REFLECT_FIELD(Player, hp)
```

展开：

```cpp
FieldInfo{
    "hp",
    offsetof(Player, hp)
}
```

所以实际上宏同时拿到了两个世界里的东西：

```cpp
源代码 token
      │
      ├────→ #field ─────→ "hp"
      │
      └────→ field ──────→ Player::hp
```

这就是很多老 C++ reflection library 的基本套路。

---

甚至你可以把声明和注册绑在一起：

```cpp
#define REFLECTABLE_FIELDS(X) \
    X(int, hp)                \
    X(float, speed)           \
    X(std::string, name)
```

然后同一份字段列表生成成员：

```cpp
#define DECLARE_FIELD(type, name) \
    type name;

struct Player {
    REFLECTABLE_FIELDS(DECLARE_FIELD)
};
```

展开成：

```cpp
struct Player {
    int hp;
    float speed;
    std::string name;
};
```

再换一个宏：

```cpp
#define MAKE_FIELD(type, name) \
    FieldInfo{#name, offsetof(Player, name)},
```

于是：

```cpp
FieldInfo fields[] = {
    REFLECTABLE_FIELDS(MAKE_FIELD)
};
```

自动变成：

```cpp
FieldInfo fields[] = {
    {"hp", offsetof(Player, hp)},
    {"speed", offsetof(Player, speed)},
    {"name", offsetof(Player, name)}
};
```

这个技巧叫 **X Macro**。

你可以把它理解成：

> **让字段的“真实声明”和“反射 metadata”共享同一个 source of truth。**

否则：

```cpp
struct Player {
    int hp;
};

// 草，忘了更新 metadata
TypeInfo PlayerInfo {
    ...
};
```
