# Compile-time Reflection

一个用于学习 C++20 静态元编程的 header-only 反射小库。它通过显式注册，为自定义类型、枚举、字段、成员函数和自由函数提供编译期名称、类型信息、遍历、查询与类型安全调用。

## 目录结构

```text
.
├── include/meta/
│   ├── core.hpp                  # 编译期字符串、注册入口与类型检测
│   ├── field.hpp                 # 字段及嵌套对象递归访问
│   ├── callable.hpp              # 成员方法与自由函数
│   ├── enum.hpp                  # 枚举项、遍历与名称转换
│   └── reflection.hpp            # 一键包含以上模块
├── examples/                     # Player 示例及可运行程序
├── tests/                        # 无第三方依赖的测试
├── docs/                         # Typst 学习材料
└── xmake.lua                     # 库、示例和测试目标
```

`examples/` 中的类型仅用于演示，不属于公共库接口。使用这个库时只需要加入 `include/` 搜索路径并包含：

```cpp
#include <meta/reflection.hpp>
```

也可以只包含需要的模块，例如 `#include <meta/enum.hpp>`。

## 类型与枚举

完整的自定义类型注册需要提供显式名称和字段 tuple：

```cpp
template <>
struct meta::reflect<Stats> {
    static constexpr auto name = meta::fixed_string{"Stats"};
    static constexpr auto fields = std::tuple{
        META_FIELD(Stats, level),
    };
};
```

随后可以使用 `meta::type_name<Stats>()`、`meta::is_reflected_v<Stats>` 和 `meta::for_each_field_recursive`。递归访问只展开显式注册的 struct/class；枚举、指针和标准容器作为叶子交给 visitor 处理。

枚举同样显式注册，因而支持负值、稀疏值和别名：

```cpp
template <>
struct meta::enum_reflect<State> {
    static constexpr auto name = meta::fixed_string{"State"};
    static constexpr auto values = std::tuple{
        META_ENUM_VALUE(State, idle),
        META_ENUM_VALUE(State, running),
    };
};
```

`meta::enum_name(value)` 和 `meta::enum_cast<State>(name)` 均返回 `std::optional`，不会为未知输入制造哨兵值。

## 构建与运行

```bash
xmake
xmake run reflection_example
xmake test
```

函数反射的完整推导见 [`docs/compile_time_callable_reflection.typ`](docs/compile_time_callable_reflection.typ)；类型、枚举与递归访问见 [`docs/type_and_enum_reflection.typ`](docs/type_and_enum_reflection.typ)。
