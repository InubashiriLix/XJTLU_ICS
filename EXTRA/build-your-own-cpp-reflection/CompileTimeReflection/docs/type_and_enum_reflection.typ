#set page(
  paper: "a4",
  margin: (x: 23mm, y: 22mm),
)
#set text(
  font: "Noto Sans CJK SC",
  size: 10.5pt,
  lang: "zh",
)
#set par(justify: true, leading: 0.72em)
#set heading(numbering: "1.")
#show raw: set text(font: "Noto Sans Mono CJK SC", size: 8.2pt)

#align(center)[
  #text(size: 20pt, weight: "bold")[补全 C++20 静态反射的类型系统]

  #v(0.5em)
  #text(size: 11pt)[自定义类型、枚举元数据与递归字段访问]
]

#v(1.2em)

字段和函数描述符解决了“这个成员叫什么、指向谁”的问题，但还没有完整回答“这个值在类型系统里是什么”。当字段变成枚举或另一个自定义对象时，成员指针仍然有效，通用算法却不知道怎样解释它。

本文在已有字段与函数反射之上补充类型层，目标包括：

- 判断一个类型是否提供了完整的反射元数据；
- 查询已注册自定义类型和枚举的名称；
- 提取字段的所属类与值类型；
- 显式注册稀疏枚举、负值和同值别名；
- 递归访问嵌套的已反射对象，同时明确容器与指针边界。

= 先分清“类型被保留”和“类型可解释”

考虑下面的字段：

```cpp
enum class State { idle, moving };
struct Stats { int level; };

struct Player {
    State state;
    Stats stats;
};
```

原有 `field_desc<"state", &Player::state>` 并没有丢失类型。`&Player::state` 的类型是 `State Player::*`，其中已经编码了所属类 `Player` 和字段类型 `State`。`get_field` 也会正确返回 `State&`。

缺少的是更高层的语义：

- `State` 有哪些合法的、具名的枚举项？
- 数值 `4` 对应哪个枚举名称？
- `Stats` 是否也注册了字段，应该继续向下遍历吗？
- `std::vector<Stats>` 应该被当成对象、序列还是不可展开的叶子？

因此不能说旧实现“不支持自定义字段类型”。更准确的结论是：它保留了静态类型，却没有为类型本身建立注册协议。

= 从成员对象指针提取字段类型

C++ 可以用偏特化拆解 `Value Owner::*`：

```cpp
template <typename T>
struct member_object_pointer_traits;

template <typename Owner, typename Value>
struct member_object_pointer_traits<Value Owner::*> {
    using owner_type = Owner;
    using value_type = Value;
};
```

将它接入字段描述符以后，每个字段同时公开三种类型：

```cpp
template <fixed_string Name, auto Member>
struct field_desc {
    static_assert(
        std::is_member_object_pointer_v<decltype(Member)>);

    static constexpr auto name = Name;
    static constexpr auto member = Member;

    using pointer_type = decltype(Member);
    using owner_type = typename
        member_object_pointer_traits<pointer_type>::owner_type;
    using value_type = typename
        member_object_pointer_traits<pointer_type>::value_type;
};
```

`static_assert` 很重要。成员函数指针同样能匹配形如 `Something Owner::*` 的语法模式，但它不应该被误注册成字段。

调用者现在可以在泛型 visitor 中根据 `Field::owner_type` 或 `Field::value_type` 编写编译期分支，而不需要从运行时字符串猜测类型。

= 给自定义类型注册名称

字段注册仍然使用 `reflect<T>`，只是在 specialization 中增加类型名：

```cpp
struct Stats {
    int level;
    float accuracy;
};

template <>
struct meta::reflect<Stats> {
    static constexpr auto name =
        meta::fixed_string{"Stats"};

    static constexpr auto fields = std::tuple{
        META_FIELD(Stats, level),
        META_FIELD(Stats, accuracy),
    };
};
```

名称仍然显式注册，而不是从编译器私有字符串中截取。这样不同编译器的行为一致，库使用者也能选择稳定的公开名称，而不必暴露完整命名空间或匿名类型细节。

== 用 requires 检测注册状态

未特化的 `reflect<T>` 是不完整类型。`requires` 可以在不制造硬编译错误的前提下检查成员是否存在：

```cpp
template <typename T>
using bare_type_t = std::remove_cvref_t<T>;

template <typename T>
concept reflected_object = requires {
    reflect<bare_type_t<T>>::name;
    reflect<bare_type_t<T>>::fields;
};

template <typename T>
inline constexpr bool is_reflected_object_v =
    reflected_object<T>;
```

要求同时存在 `name` 和 `fields`，意味着这个 concept 表示“完整的对象类型元数据”。为了兼容早期代码，只依赖 `fields` 的 `get_field` 和 `for_each_field` 不需要强制类型名；只有统一类型查询和递归遍历要求完整注册。

= 为什么枚举需要单独的注册表

enum 没有可遍历的数据成员，不能复用 `reflect<T>::fields`。它需要描述的是一组名称和值：

```cpp
template <fixed_string Name, auto Value>
struct enum_value_desc {
    static_assert(std::is_enum_v<decltype(Value)>);

    static constexpr auto name = Name;
    static constexpr auto value = Value;

    using enum_type = decltype(Value);
    using underlying_type =
        std::underlying_type_t<enum_type>;
};
```

相应的 customization point 是 `enum_reflect<E>`：

```cpp
enum class PlayerState : int {
    idle = -1,
    moving = 4,
    dead = 9,
    knocked_out = dead,
};

template <>
struct meta::enum_reflect<PlayerState> {
    static constexpr auto name =
        meta::fixed_string{"PlayerState"};

    static constexpr auto values = std::tuple{
        META_ENUM_VALUE(PlayerState, idle),
        META_ENUM_VALUE(PlayerState, moving),
        META_ENUM_VALUE(PlayerState, dead),
        META_ENUM_VALUE(PlayerState, knocked_out),
    };
};
```

这里不能用枚举底层值直接作为数组下标：`idle` 是负数，`moving` 和 `dead` 之间存在空洞，`dead` 与 `knocked_out` 还共享同一个值。异构 tuple 描述符不依赖数值连续性，因此这些情况都可以自然表达。

== 为什么选择显式注册

有些库会实例化一段整数范围，再解析 `__PRETTY_FUNCTION__` 或 `__FUNCSIG__` 判断某个整数是不是具名枚举项。这种方法很方便，但它依赖编译器输出格式，也必须猜测扫描上下界。

显式注册虽然多写几行，却具有更明确的性质：

- 不依赖 GCC、Clang 或 MSVC 的私有字符串格式；
- 稀疏值不增加扫描成本；
- 不会漏掉扫描范围外的合法值；
- 可以明确选择公开名称和别名顺序。

= 枚举元数据校验

每个枚举描述符都应该属于正在注册的枚举，并且反射名称必须唯一。可以在编译期遍历 tuple 类型进行检查。

同值别名不应被判为错误。C++ 允许不同 enumerator 共享底层值，注册表顺序负责定义 value-to-name 的规范名称：

```text
dead        -> 9
knocked_out -> 9

enum_name(PlayerState::knocked_out) -> "dead"
enum_cast<PlayerState>("knocked_out") -> PlayerState::dead
```

第二个结果在数值上也等于 `PlayerState::knocked_out`；C++ 运行时值本身无法记录它最初使用了哪个别名。

= 使用 optional 表达未知值

强制把任意整数转换成 enum 在 C++ 中是允许的，因此运行时可能出现未注册值。名称输入也可能拼写错误。返回 `std::optional` 比空字符串或虚构的 `unknown` enumerator 更诚实：

```cpp
auto name = meta::enum_name(PlayerState::moving);
// optional<string_view>{"moving"}

auto state = meta::enum_cast<PlayerState>("idle");
// optional<PlayerState>{PlayerState::idle}

auto missing = meta::enum_name(
    static_cast<PlayerState>(123));
// nullopt
```

核心循环按显式注册顺序检查描述符。找到结果以后仍可继续展开编译期 fold，但通过 `if (!result)` 保证第一个匹配名称不会被后面的别名覆盖。

= 统一类型名称

对象和 enum 使用不同注册表，但使用者不应该为类型名编写两套代码。统一接口可以在编译期分支：

```cpp
template <typename T>
consteval std::string_view type_name() {
    using type = std::remove_cvref_t<T>;
    static_assert(is_reflected_v<type>);

    if constexpr (is_reflected_object_v<type>) {
        return reflect<type>::name.view();
    } else {
        return enum_reflect<type>::name.view();
    }
}
```

`type_name<int>()` 不返回编译器相关的字符串，而是产生明确的编译期错误：基础类型没有显式注册的公开名称。上层应用若需要基础类型名称，可以另行提供自己的映射策略。

= 递归访问嵌套自定义对象

`for_each_field` 只看一层。递归版本对每个字段执行以下步骤：

1. 通过成员对象指针取得字段引用；
2. 先回调 `(descriptor, value, depth)`；
3. 如果字段值类型满足 `reflected_object`，再进入它的字段。

核心形态如下：

```cpp
template <typename T, typename Visitor>
constexpr void recursive_impl(
    T& object, Visitor& visitor, std::size_t depth)
{
    std::apply(
        [&]<typename... Fields>(Fields... fields) {
            auto visit = [&](auto field) {
                using field_type = decltype(field);
                auto&& value = std::invoke(
                    field_type::member, object);

                std::invoke(visitor, field, value, depth);

                using value_type =
                    std::remove_cvref_t<decltype(value)>;
                if constexpr (
                    is_reflected_object_v<value_type>) {
                    recursive_impl(value, visitor, depth + 1);
                }
            };

            (visit(fields), ...);
        },
        reflect<std::remove_cvref_t<T>>::fields);
}
```

如果 `Player::stats` 是已注册的 `Stats`，访问顺序类似：

```text
hp       depth 0
speed    depth 0
name     depth 0
stats    depth 0
level    depth 1
accuracy depth 1
state    depth 0
```

父字段也会回调，所以调用者可以构建树形结构、路径或对象边界。API 只接受左值或 const 左值，避免 visitor 先移动一个临时复合字段，递归过程随后又访问已经移动的对象。

== const 如何传播

当根对象是 `const Player&` 时，成员指针访问得到 `const Stats&`，继续递归后得到 `const int&` 和 `const float&`。实现没有复制对象，也没有去除 const。

visitor 使用泛型 lambda 即可观察这种性质：

```cpp
meta::for_each_field_recursive(
    const_player,
    []<typename Field>(
        Field, auto& value, std::size_t depth)
    {
        static_assert(std::is_const_v<
            std::remove_reference_t<decltype(value)>>);
    });
```

= 为什么容器和指针暂时是叶子

对 `std::vector<Stats>` 自动递归看似自然，但它立即引入额外策略：索引如何进入路径、空容器怎样表示、map 的 key 是否访问、指针是否允许为空、智能指针是否代表所有权，以及循环对象图如何终止。

第一版只递归“字段值本身就是已注册对象”的情况：

- `Stats`：递归；
- `PlayerState`：叶子，用 enum API 解释；
- `Stats*`：叶子；
- `std::vector<Stats>`：叶子；
- 未注册第三方类型：叶子。

visitor 仍然取得这些叶子的真实类型，可以在上层按项目需求增加容器、指针或序列化策略，而不让核心反射库替用户猜测所有权。

= 模块化头文件

类型层加入后，所有内容继续堆在单个实现文件会降低可读性。公共头文件按职责拆分：

```text
meta/core.hpp       fixed_string、注册入口、concept、type_name
meta/field.hpp      字段描述符、字段算法、递归访问
meta/callable.hpp   函数 traits、注册表、查询与调用
meta/enum.hpp       枚举描述符、校验、遍历与转换
meta/reflection.hpp 一键包含入口
```

已有代码继续包含 `<meta/reflection.hpp>`，无需迁移。只需要 enum 的轻量使用者也可以直接包含 `<meta/enum.hpp>`。每个模块必须能够被单独包含和编译，不能偷偷依赖总入口的包含顺序。

= 使用示例

一个 visitor 可以同时处理嵌套对象、enum 和普通叶子：

```cpp
meta::for_each_field_recursive(
    player,
    []<typename Field>(
        Field, const auto& value, std::size_t depth)
    {
        using value_type =
            std::remove_cvref_t<decltype(value)>;

        std::cout << std::string(depth * 2, ' ')
                  << Field::name.view() << " = ";

        if constexpr (
            meta::is_reflected_object_v<value_type>) {
            std::cout << '<'
                      << meta::type_name<value_type>()
                      << '>';
        } else if constexpr (
            meta::is_enum_reflected_v<value_type>) {
            std::cout << meta::enum_name(value)
                             .value_or("<unknown>");
        } else {
            std::cout << value;
        }

        std::cout << '\n';
    });
```

这里的反射核心只负责提供结构和强类型引用；“如何打印”仍然是 visitor 的策略。相同元数据可以被调试界面、配置绑定、编辑器面板、RPC 描述或序列化器复用。

= 设计边界与后续方向

当前类型层有意保留以下边界：

- 注册是显式的，C++20 库不能自动枚举任意类的成员；
- 基础类型名称不从编译器私有字符串推导；
- enum 名称和值互转只覆盖已注册项；
- 同值 enum 别名的规范名称由注册顺序决定；
- 递归访问不自动解引用指针，也不自动展开标准容器；
- JSON、文本格式和错误恢复属于元数据之上的独立层。

适合作为下一步练习的方向包括：

1. 为容器定义独立的 `sequence_traits`，由上层 visitor 选择是否展开；
2. 为字段描述符加入属性，例如显示名、范围、必填或文档字符串；
3. 构造只读的字段路径对象，而不是在 visitor 中临时拼接字符串；
4. 为 enum 增加 flags 位集合策略，但不要把普通 enum 自动当成位掩码；
5. 基于同一元数据实现一个独立的小型序列化示例，验证核心接口是否足够通用。

= 总结

支持 enum 和自定义类型并不是在 `for_each_field` 中增加几个 `if`。关键是承认两层信息的区别：成员指针保留了 C++ 静态类型，而反射注册表赋予类型可查询的语义。

自定义对象通过 `reflect<T>` 描述名称和字段，enum 通过 `enum_reflect<E>` 描述名称和值，统一 traits 判断它们是否完整注册。递归 visitor 只展开明确注册的对象，并把所有策略不确定的类型留作叶子。这样得到的类型反射仍然小、可移植、类型安全，同时为更高层工具提供了稳定基础。
