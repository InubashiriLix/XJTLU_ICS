#set page(
  paper: "a4",
  margin: (x: 23mm, y: 22mm),
)
#set text(
  font: "FandolSong",
  size: 10.5pt,
  lang: "zh",
)
#set par(justify: true, leading: 0.72em)
#set heading(numbering: "1.")
#show raw: set text(font: "Maple Mono NF", size: 8.2pt)

#align(center)[
  #text(size: 20pt, weight: "bold")[从字段到函数：手写 C++20 编译时反射]

  #v(0.5em)
  #text(size: 11pt)[成员函数、自由函数、重载与类型安全调用]
]

#v(1.2em)

本文不是在模拟运行时脚本语言的“万能反射”。我们的目标更小，也更符合 C++ 的类型系统：程序员显式注册实体，编译器在编译期保存名称和指针，并借助模板完成遍历、查询及调用。

读完后，你应该能够回答四个问题：

- 字段描述符为什么可以自然扩展成函数描述符？
- 如何从函数指针和成员函数指针中提取返回类型、参数类型、`const` 与 `noexcept`？
- 为什么重载函数注册时必须先消歧？
- 如何让按名称查找仍然保持完整的静态类型检查？

= 先看现有实现建立了什么

当前项目中的字段反射可以归纳成三层。

== 编译期字符串

`fixed_string` 把字符串字面量装进结构体，使字符串能够作为 C++20 非类型模板参数（NTTP）：

```cpp
template <std::size_t N>
struct fixed_string {
    char data[N];

    constexpr fixed_string(const char (&text)[N]) noexcept {
        std::copy_n(text, N, data);
    }

    constexpr std::string_view view() const noexcept {
        return {data, N - 1};
    }
};
```

于是 `field_desc<"hp", &Player::hp>` 中的 `"hp"` 不再只是运行时数据，而是描述符类型的一部分。两个名称不同的描述符也是两个不同的 C++ 类型。

== 描述符

字段描述符同时保存“叫什么”和“指向谁”：

```cpp
template <fixed_string Name, auto Member>
struct field_desc {
    static constexpr auto name = Name;
    static constexpr auto member = Member;
};
```

`&Player::hp` 的类型是 `int Player::*`，即“指向 `Player` 的 `int` 成员的指针”。它不是字节偏移量，也不是普通地址。编译器知道它只能和合适的对象一起使用：

```cpp
Player player{};
int Player::* pointer = &Player::hp;
player.*pointer = 100;
```

== 注册表与算法

`reflect<Player>::fields` 是由若干不同描述符组成的 `std::tuple`。tuple 很重要，因为不同字段通常具有不同类型，不能放进普通的同类型数组。

```cpp
template <>
struct meta::reflect<Player> {
    static constexpr auto fields = std::tuple{
        META_FIELD(Player, hp),
        META_FIELD(Player, speed),
        META_FIELD(Player, name),
    };
};
```

反射算法并没有扫描 `Player` 的定义。它只是在程序员显式给出的 tuple 上运行。因此，这套设计更准确的名字是“编译期静态元信息注册系统”。

= 现有 `has` 为什么有问题

项目中的 `has` 没有在示例里实例化，所以当前程序仍能编译；一旦真正调用，它会暴露几个问题。

第一，`tuple_size` 应该作用于 `reflect<T>::fields` 的类型，而不是 `Player` 本身。第二，`std::find_if` 需要一对同类型迭代器，但 `std::tuple` 不是普通容器，没有 `begin` 和 `end`。第三，`find_if` 返回迭代器而非 `bool`。

tuple 的处理方式是 `std::apply` 加 fold expression：

```cpp
template <fixed_string Name, typename T>
consteval bool has_field() {
    using type = std::remove_cvref_t<T>;
    return std::apply(
        []<typename... Fields>(Fields...) {
            return (false || ... ||
                    (Fields::name.view() == Name.view()));
        },
        reflect<type>::fields);
}
```

这里在最左边放一个 `false`，可以让空 tuple 的折叠结果也有定义。这类算法只依赖描述符的静态成员，不需要创建真实对象，因此用 `has_field<"hp", Player>()` 比接收一个未使用的 `Player` 参数更贴近其语义。

= 从字段指针走向可调用指针

字段描述符保存 `&Type::field`。函数描述符可以使用同样的结构保存以下两类指针：

```cpp
int (*free_function)(double);
int (Player::*member_function)(double) const noexcept;
```

两者看起来相似，语义却不同。

- 普通函数指针已经包含返回类型和参数类型，调用时只需要函数参数。
- 成员函数指针还包含所属类及成员限定符，调用时必须额外提供对象。
- 静态成员函数没有隐含的 `this`，取地址以后就是普通函数指针。

我们可以让同一个描述符模板保存它们：

```cpp
template <fixed_string Name, auto Pointer>
struct callable_desc {
    static constexpr auto name = Name;
    static constexpr auto pointer = Pointer;

    using pointer_type = decltype(Pointer);
    using traits = callable_traits<pointer_type>;
};
```

`auto` 非类型模板参数会保留指针的精确类型。真正困难的工作转移到了 `callable_traits`：给定指针类型，拆出其中的元信息。

= 用偏特化拆解函数类型

主模板只做声明，不给默认实现。这样传入不支持的类型会在编译期明确失败，而不是产生看似合理的错误元数据。

```cpp
template <typename T>
struct callable_traits;
```

== 普通函数指针

`R (*)(Args...)` 是一个可被偏特化匹配的类型模式：

```cpp
template <typename R, typename... Args>
struct callable_traits<R (*)(Args...)> {
    using return_type = R;
    using args_type = std::tuple<Args...>;
    using class_type = void;

    static constexpr std::size_t arity = sizeof...(Args);
    static constexpr bool is_member = false;
    static constexpr bool is_const = false;
    static constexpr bool is_noexcept = false;
};
```

如果函数声明为 `noexcept`，它的指针类型也是不同的类型，所以还需要 `R (*)(Args...) noexcept` 的特化。

== 成员函数指针

成员函数的基础模式是 `R (C::*)(Args...)`。`C` 是所属类，`R` 是返回类型，`Args...` 是显式参数；隐含的 `this` 不计入 `arity`。

常用的四个组合分别是：

```cpp
R (C::*)(Args...)
R (C::*)(Args...) const
R (C::*)(Args...) noexcept
R (C::*)(Args...) const noexcept
```

教学实现先支持这四类。若未来需要 `volatile`、`&`、`&&` 或 C 风格变参，可以继续增加偏特化；这正是 traits 设计的扩展点。

为了避免四份实现重复，可以先建立一个只负责保存结果的基类，再让每个偏特化传入布尔属性。文末完整代码展示了这种写法。

= 调用统一交给 `std::invoke`

字段访问需要 `.*` 或 `->*`，成员方法调用还需要 `(object.*pointer)(args...)`。如果把普通函数、成员函数、对象和指针的不同组合全部手写一遍，代码很快会被语法细节淹没。

C++17 起提供的 `std::invoke` 已经统一了这些规则。因此描述符只需要转发参数：

```cpp
template <typename... Args>
static constexpr decltype(auto) invoke(Args&&... args)
    noexcept(std::is_nothrow_invocable_v<pointer_type, Args...>)
{
    return std::invoke(pointer, std::forward<Args>(args)...);
}
```

这里有三个值得注意的细节。

- `decltype(auto)` 保留返回引用，不会像单纯的 `auto` 那样意外复制。
- 完美转发保留左值、右值和 cv 限定。
- `is_nothrow_invocable_v` 根据实际传入的对象与参数计算包装函数是否为 `noexcept`。

描述符并没有做类型擦除。参数不匹配时，错误发生在编译期；返回类型也仍然是原函数的真实类型。

= 注册成员方法与自由函数

== 成员方法属于被反射类型

成员方法自然放进 `reflect<T>::methods`：

```cpp
template <>
struct meta::reflect<Player> {
    static constexpr auto methods = std::tuple{
        META_METHOD(Player, heal),
        META_METHOD(Player, health),
    };
};
```

字段和方法使用两个 tuple，而不是硬塞进同一个 tuple。这样字段算法不会误接收函数描述符，接口语义也更清楚。

== 自由函数使用标签分组

自由函数没有天然的所属类。如果把它们放进唯一的全局 tuple，不同模块很容易出现同名和依赖问题。一个简单办法是让使用者定义空标签类型：

```cpp
struct game_api;

template <>
struct meta::function_registry<game_api> {
    static constexpr auto functions = std::tuple{
        META_FUNCTION(total_score),
        META_FUNCTION(describe),
    };
};
```

标签不需要对象，也不占运行时空间。它只是编译期命名空间，使 `game_api`、`network_api` 等注册表彼此隔离。

= 重载：名字不足以确定函数

考虑下面两个成员函数：

```cpp
void set_speed(float value);
void set_speed(int value);
```

表达式 `&Player::set_speed` 仍是一个重载集合，编译器无法从 `auto` 推断该选择哪一个。必须先把它转换成精确的成员函数指针类型：

```cpp
static_cast<void (Player::*)(float)>(&Player::set_speed)
```

注册宏只是把这段显式消歧包装起来：

```cpp
#define META_METHOD_SIG(Type, Method, Signature) \
    ::meta::callable_desc<                         \
        #Method,                                   \
        static_cast<Signature>(&Type::Method)> {}
```

自由函数重载同理，精确类型形如 `int (*)(double)`。

同一个注册表可以保存多个同名描述符。查找时只给名称仍然不够，因此本教程使用“名称 + 精确指针签名”作为键：

```cpp
meta::invoke_method<
    "set_speed",
    void (Player::*)(float)
>(player, 2.5f);
```

这比“根据传入实参猜重载”略长，但规则完全确定，不会因为隐式数值转换或后来新增重载而悄悄改变行为。

= 在异构 tuple 中查询

`has_callable` 可以用与字段相同的折叠方式，同时比较名称和指针类型：

```cpp
template <fixed_string Name, typename Signature, typename Tuple>
consteval bool has_callable(const Tuple& registry) {
    return std::apply(
        []<typename... Items>(Items...) {
            return (false || ... ||
                    (Items::name.view() == Name.view() &&
                     std::is_same_v<typename Items::pointer_type,
                                    Signature>));
        },
        registry);
}
```

`get_callable` 不能使用普通运行时循环，因为 tuple 中每个位置的返回类型都可能不同。解决办法是以索引 `I` 递归，并用 `if constexpr` 在编译期只保留命中的分支：

```cpp
template <fixed_string Name, typename Signature,
          std::size_t I = 0, typename Tuple>
constexpr decltype(auto) get_callable(Tuple&& registry) {
    using tuple_type = std::remove_cvref_t<Tuple>;

    if constexpr (I == std::tuple_size_v<tuple_type>) {
        static_assert(I != std::tuple_size_v<tuple_type>,
                      "reflection callable not found");
    } else {
        using item = std::tuple_element_t<I, tuple_type>;
        if constexpr (item::name.view() == Name.view() &&
                      std::is_same_v<typename item::pointer_type,
                                     Signature>) {
            return std::get<I>(std::forward<Tuple>(registry));
        } else {
            return get_callable<Name, Signature, I + 1>(
                std::forward<Tuple>(registry));
        }
    }
}
```

如果没有命中，递归最终实例化带有清晰消息的 `static_assert`。如果命中，未选择的递归分支不会参与最终程序。

= 面向使用者的调用接口

底层 `get_callable` 可以处理任意描述符 tuple。上层接口只负责选择正确注册表，并把参数交给描述符。

成员方法接口从对象类型定位 `reflect<T>::methods`：

```cpp
template <fixed_string Name, typename Signature,
          typename T, typename... Args>
constexpr decltype(auto)
invoke_method(T&& object, Args&&... args) {
    using type = std::remove_cvref_t<T>;
    auto&& descriptor = get_callable<Name, Signature>(
        reflect<type>::methods);
    return descriptor.invoke(
        std::forward<T>(object),
        std::forward<Args>(args)...);
}
```

自由函数接口则从标签定位 `function_registry<Tag>::functions`。它不需要对象参数。

遍历仍然使用 `std::apply`。调用者可以用泛型 lambda 读取每个描述符的类型级元信息：

```cpp
meta::for_each_callable(
    meta::reflect<Player>::methods,
    []<typename Descriptor>(Descriptor) {
        std::cout << Descriptor::name.view()
                  << ", arity="
                  << Descriptor::traits::arity << '\n';
    });
```

= 完整可编译示例

下面的程序把本教程的核心机制集中在一个翻译单元中。它不依赖当前项目源码，可以单独使用 C++20 编译器验证。

```cpp
// BEGIN_COMPILE_TEST
#include <algorithm>
#include <cstddef>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#define META_FIELD(Type, Member) \
    ::meta::field_desc<#Member, &Type::Member> {}

#define META_METHOD(Type, Method) \
    ::meta::callable_desc<#Method, &Type::Method> {}

#define META_METHOD_SIG(Type, Method, Signature) \
    ::meta::callable_desc<                         \
        #Method,                                   \
        static_cast<Signature>(&Type::Method)> {}

#define META_FUNCTION(Function) \
    ::meta::callable_desc<#Function, &Function> {}

#define META_FUNCTION_SIG(Function, Signature) \
    ::meta::callable_desc<                          \
        #Function,                                  \
        static_cast<Signature>(&Function)> {}

namespace meta {

template <std::size_t N>
struct fixed_string {
    char data[N];

    constexpr fixed_string(const char (&text)[N]) noexcept {
        std::copy_n(text, N, data);
    }

    constexpr std::string_view view() const noexcept {
        return {data, N - 1};
    }
};

template <std::size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N>;

template <fixed_string Name, auto Member>
struct field_desc {
    static constexpr auto name = Name;
    static constexpr auto member = Member;
};

template <typename T>
struct reflect;

template <typename Tag>
struct function_registry;

template <typename Class, typename Return,
          bool IsMember, bool IsConst, bool IsNoexcept,
          typename... Args>
struct callable_traits_base {
    using class_type = Class;
    using return_type = Return;
    using args_type = std::tuple<Args...>;

    static constexpr std::size_t arity = sizeof...(Args);
    static constexpr bool is_member = IsMember;
    static constexpr bool is_const = IsConst;
    static constexpr bool is_noexcept = IsNoexcept;
};

template <typename T>
struct callable_traits;

template <typename R, typename... Args>
struct callable_traits<R (*)(Args...)>
    : callable_traits_base<void, R, false, false, false,
                           Args...> {};

template <typename R, typename... Args>
struct callable_traits<R (*)(Args...) noexcept>
    : callable_traits_base<void, R, false, false, true,
                           Args...> {};

template <typename C, typename R, typename... Args>
struct callable_traits<R (C::*)(Args...)>
    : callable_traits_base<C, R, true, false, false,
                           Args...> {};

template <typename C, typename R, typename... Args>
struct callable_traits<R (C::*)(Args...) const>
    : callable_traits_base<C, R, true, true, false,
                           Args...> {};

template <typename C, typename R, typename... Args>
struct callable_traits<R (C::*)(Args...) noexcept>
    : callable_traits_base<C, R, true, false, true,
                           Args...> {};

template <typename C, typename R, typename... Args>
struct callable_traits<R (C::*)(Args...) const noexcept>
    : callable_traits_base<C, R, true, true, true,
                           Args...> {};

template <fixed_string Name, auto Pointer>
struct callable_desc {
    static constexpr auto name = Name;
    static constexpr auto pointer = Pointer;

    using pointer_type = decltype(Pointer);
    using traits = callable_traits<pointer_type>;

    template <typename... Args>
    static constexpr decltype(auto) invoke(Args&&... args)
        noexcept(std::is_nothrow_invocable_v<
                 pointer_type, Args...>)
    {
        return std::invoke(
            pointer, std::forward<Args>(args)...);
    }
};

template <fixed_string Name, typename T>
consteval bool has_field() {
    using type = std::remove_cvref_t<T>;
    return std::apply(
        []<typename... Fields>(Fields...) {
            return (false || ... ||
                    (Fields::name.view() == Name.view()));
        },
        reflect<type>::fields);
}

template <fixed_string Name, typename Signature,
          typename Tuple>
consteval bool has_callable(const Tuple& registry) {
    return std::apply(
        []<typename... Items>(Items...) {
            return (false || ... ||
                    (Items::name.view() == Name.view() &&
                     std::is_same_v<
                         typename Items::pointer_type,
                         Signature>));
        },
        registry);
}

template <fixed_string Name, typename Signature,
          std::size_t I = 0, typename Tuple>
constexpr decltype(auto) get_callable(Tuple&& registry) {
    using tuple_type = std::remove_cvref_t<Tuple>;
    constexpr auto count = std::tuple_size_v<tuple_type>;

    if constexpr (I == count) {
        static_assert(I != count,
                      "reflection callable not found");
    } else {
        using item = std::tuple_element_t<I, tuple_type>;
        if constexpr (
            item::name.view() == Name.view() &&
            std::is_same_v<typename item::pointer_type,
                           Signature>) {
            return std::get<I>(
                std::forward<Tuple>(registry));
        } else {
            return get_callable<Name, Signature, I + 1>(
                std::forward<Tuple>(registry));
        }
    }
}

template <typename Tuple, typename Fn>
constexpr void for_each_callable(
    const Tuple& registry, Fn&& function)
{
    std::apply(
        [&]<typename... Items>(const Items&... items) {
            (std::invoke(function, items), ...);
        },
        registry);
}

template <fixed_string Name, typename Signature,
          typename T, typename... Args>
constexpr decltype(auto)
invoke_method(T&& object, Args&&... args) {
    using type = std::remove_cvref_t<T>;
    auto&& descriptor = get_callable<Name, Signature>(
        reflect<type>::methods);
    return descriptor.invoke(
        std::forward<T>(object),
        std::forward<Args>(args)...);
}

template <typename Tag, fixed_string Name,
          typename Signature, typename... Args>
constexpr decltype(auto)
invoke_function(Args&&... args) {
    auto&& descriptor = get_callable<Name, Signature>(
        function_registry<Tag>::functions);
    return descriptor.invoke(
        std::forward<Args>(args)...);
}

} // namespace meta

struct Player {
    int hp;
    float speed;
    std::string name;

    void heal(int amount) {
        hp += amount;
    }

    void set_speed(float value) {
        speed = value;
    }

    void set_speed(int value) {
        speed = static_cast<float>(value);
    }

    int health() const noexcept {
        return hp;
    }
};

template <>
struct meta::reflect<Player> {
    static constexpr auto fields = std::tuple{
        META_FIELD(Player, hp),
        META_FIELD(Player, speed),
        META_FIELD(Player, name),
    };

    static constexpr auto methods = std::tuple{
        META_METHOD(Player, heal),
        META_METHOD_SIG(
            Player, set_speed,
            void (Player::*)(float)),
        META_METHOD_SIG(
            Player, set_speed,
            void (Player::*)(int)),
        META_METHOD(Player, health),
    };
};

int total_score(const Player& player, int bonus) noexcept {
    return player.hp + bonus;
}

std::string describe(const Player& player) {
    return player.name + ":" + std::to_string(player.hp);
}

struct game_api;

template <>
struct meta::function_registry<game_api> {
    static constexpr auto functions = std::tuple{
        META_FUNCTION(total_score),
        META_FUNCTION(describe),
    };
};

int main() {
    static_assert(meta::has_field<"hp", Player>());
    static_assert(!meta::has_field<"level", Player>());

    static_assert(meta::has_callable<
        "set_speed", void (Player::*)(float)>(
            meta::reflect<Player>::methods));

    using health_pointer = int (Player::*)() const noexcept;
    using health_desc = std::remove_cvref_t<decltype(
        meta::get_callable<"health", health_pointer>(
            meta::reflect<Player>::methods))>;
    static_assert(health_desc::traits::is_member);
    static_assert(health_desc::traits::is_const);
    static_assert(health_desc::traits::is_noexcept);
    static_assert(health_desc::traits::arity == 0);

    Player player{80, 1.0f, "Ada"};

    meta::invoke_method<
        "heal", void (Player::*)(int)>(player, 15);
    meta::invoke_method<
        "set_speed", void (Player::*)(float)>(
            player, 2.5f);

    const int hp = meta::invoke_method<
        "health", health_pointer>(player);

    const int score = meta::invoke_function<
        game_api,
        "total_score",
        int (*)(const Player&, int) noexcept>(
            player, 5);

    meta::for_each_callable(
        meta::reflect<Player>::methods,
        []<typename Descriptor>(Descriptor) {
            std::cout
                << Descriptor::name.view()
                << ": arity="
                << Descriptor::traits::arity
                << '\n';
        });

    std::cout << "hp=" << hp
              << ", speed=" << player.speed
              << ", score=" << score << '\n';
}
// END_COMPILE_TEST
```

运行结果中的方法遍历会保留两个同名的 `set_speed`，因为它们是两个不同签名的描述符。最终状态应为 `hp=95`、`speed=2.5`、`score=100`。

= 设计边界与继续扩展

== 这套实现保证了什么

- 名称在编译期参与查找，不需要运行时哈希表。
- 指针和签名始终保留真实类型，没有 `void*` 或 `std::any`。
- 错误的对象类型、参数类型和返回类型会由编译器拒绝。
- 描述符通常是空类型，注册 tuple 不需要保存运行时字符串或动态分配内存。

== 它没有自动做到什么

- C++20 不能让普通库枚举一个类的所有成员；注册仍然是显式的。
- 外部代码不能通过取地址绕过 `private` 或 `protected`。若确实需要注册私有方法，应由类内部提供元信息或显式声明友元，而不是破坏访问控制。
- 静态成员函数的指针类型与自由函数相同。可以把它放进类的 `methods` tuple 表达逻辑归属，但 traits 只能报告它不是非静态成员函数。
- 当前 traits 没覆盖 `volatile`、引用限定成员函数、变参函数及函数对象。它们可以扩展，但不应在没有需求时一次性堆入教学核心。
- 按运行时字符串调用需要类型擦除、参数封装、返回值协议和错误处理，是另一套系统，不应与这里的编译期查找混为一谈。

== 可以练习的扩展

1. 为 `callable_traits` 增加 `&` 和 `&&` 限定的成员函数偏特化，并测试对象值类别。
2. 增加 `argument_t<I>`，通过 `std::tuple_element_t<I, args_type>` 查询第 `I` 个参数类型。
3. 给描述符增加用户自定义属性 tuple，例如文档、权限或序列化标签。
4. 写一个 `consteval` 检查，禁止同一注册表出现完全相同的“名称 + 签名”。
5. 将字段与函数共享的名称查找部分抽成更通用的 tuple 元算法，同时保留字段和函数的上层语义接口。

= 总结

这套函数反射并没有依靠编译器魔法。它只组合了几项普通但强大的 C++ 能力：编译期字符串、非类型模板参数、指针的静态类型、模板偏特化、异构 tuple、fold expression、`if constexpr` 与 `std::invoke`。

字段反射和函数反射的共同核心是：先把实体变成“名称 + 强类型句柄”的描述符，再把描述符放入编译期注册表，最后编写只在这些注册表上工作的算法。只要始终保留精确类型，并明确处理重载和所属范围，这个小型系统就既适合学习元编程，也足以成为更大型静态元信息工具的基础。
