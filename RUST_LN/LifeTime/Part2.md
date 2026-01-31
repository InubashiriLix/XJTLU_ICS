## 1) Lifetime elision：三条省略规则（编译器帮你补 `'a` 的规则）

> Elision 只发生在**函数签名**里（参数/返回值），不是函数体里。
> 目的：你不写 lifetime，编译器按规则“自动推断并补全”。

### 规则 1：每个“引用参数”各自获得一个独立的 lifetime 参数

```rust
fn f(x: &i32, y: &i32) -> &i32
```

等价于（先别管能不能通过，这是规则层面）：

```rust
fn f<'a, 'b>(x: &'a i32, y: &'b i32) -> &i32
```

也就是：**有几个引用入参，就先假设有几个 lifetime。**

---

### 规则 2：如果只有一个引用参数，那么返回值引用就用它的 lifetime

```rust
fn first(s: &str) -> &str
```

等价于：

```rust
fn first<'a>(s: &'a str) -> &'a str
```

**只要入参里只有一个引用，返回引用就“跟它走”。**

---

### 规则 3：如果有多个引用参数，但其中一个是 `&self / &mut self`，则返回值引用用 `self` 的 lifetime

典型方法签名：

```rust
impl Foo {
    fn get(&self) -> &str { ... }
}
```

等价于：

```rust
impl Foo {
    fn get<'a>(&'a self) -> &'a str { ... }
}
```

**只要是方法并且出现 `&self`，返回引用默认绑定到 `self`。**

---

### 什么时候会 elision 失败？

当**返回引用**无法唯一确定该绑定到哪个输入引用时。

比如：

```rust
fn pick(x: &str, y: &str) -> &str {
    if x.len() > y.len() { x } else { y }
}
```

这里两个入参都是引用，返回引用可能来自 x 或 y，**规则 2 不适用**，规则 3 也不适用（不是 `self`），所以编译器会让你写：

```rust
fn pick<'a>(x: &'a str, y: &'a str) -> &'a str { ... }
```

含义是：**要求 x 和 y 至少活到同一个 `'a`，返回值也活到 `'a`。**

---

## 2) 如何“设计 API 来避免 lifetime”——核心思路

> 本质就一句：**别让返回值“借用”入参**，改成“拥有(own)”或“借用但只在局部使用”。

下面是最常用、最工程化的几类招式（按推荐程度从高到低）。

---

### A) 让 API 返回“拥有的值”（Own the data）

这是最简单、最稳、最少 lifetime 的方案：返回 `String / Vec / OwnedStruct`。

**坏：**（需要 lifetime）

```rust
fn extract_name(line: &str) -> &str { ... }
```

**好：**（不要 lifetime）

```rust
fn extract_name(line: &str) -> String {
    // 需要的内容复制/构造出来
    line.split(',').next().unwrap_or("").to_string()
}
```

什么时候该用它：

* 你做的是解析/提取/处理，返回结果会被长期保存
* 性能不是极限（或字符串很短）

---

### B) 用 `Cow<'a, str>`：能借就借、要改才拷贝（“性感的折中”）

`Cow` 允许你返回**可能借用也可能拥有**的字符串/切片：

* 不需要修改 → 借用 `&'a str`（零拷贝）
* 需要修改/拼接 → 转成 `String`（拷贝）

```rust
use std::borrow::Cow;

fn normalize<'a>(s: &'a str) -> Cow<'a, str> {
    if s.chars().all(|c| c.is_ascii_lowercase()) {
        Cow::Borrowed(s)
    } else {
        Cow::Owned(s.to_ascii_lowercase())
    }
}
```

你会看到：这里还是有 `'a`，但它经常**比“到处写引用返回”更好用**，因为调用者拿到的是一个“值”，更容易存/传。

---

### C) 让“借用”发生在闭包里：把 lifetime 限制在调用期间（HRTB/回调风格）

如果你的 API 只需要临时借用数据，最干净的方法是：
**不要返回引用**，而是让用户提供一个闭包，你在闭包里把引用给他用。

```rust
fn with_line<R>(input: &str, f: impl FnOnce(&str) -> R) -> R {
    let line = input.lines().next().unwrap_or("");
    f(line) // 引用只活在这次调用中
}
```

调用者：

```rust
let len = with_line(text, |line| line.len());
```

优点：

* 你几乎不需要在公开 API 上写 lifetime
* 引用绝不会逃逸（所以安全、简单）

---

### D) 让类型“拥有数据”，方法返回 `&self` 的切片（利用规则 3）

这招很 Rust：**构造时把东西存进 struct**，后续方法返回引用都绑定到 `self`，用户基本不需要显式 lifetime。

```rust
struct Parser {
    buf: String,
}

impl Parser {
    fn new(buf: String) -> Self { Self { buf } }

    fn header(&self) -> &str {
        self.buf.lines().next().unwrap_or("")
    }
}
```

你看：`header(&self) -> &str` 这里用户不用写 `'a`，因为 elision 规则 3 自动绑定到 `self`。

---

### E) 用 `Arc/Box/Rc`：把“共享 + 生命周期”交给所有权系统

当你需要跨线程/跨模块长期保存共享数据时，别纠结 lifetime：

* 单线程共享：`Rc<T>`
* 多线程共享：`Arc<T>`
* 想把大对象放堆上：`Box<T>`

例子（多处共享配置）：

```rust
use std::sync::Arc;

#[derive(Clone)]
struct Ctx {
    config: Arc<String>,
}

impl Ctx {
    fn config(&self) -> &str {
        &self.config
    }
}
```

---

### F) 用索引/ID 代替引用（避免把借用穿透 API 边界）

尤其你写“管理系统 / 组织管理 / ECS / 资源管理器”这种：
**返回一个句柄（ID）**，需要用的时候再 `get(id)`。

```rust
struct Store {
    items: Vec<String>,
}

#[derive(Copy, Clone)]
struct Id(usize);

impl Store {
    fn insert(&mut self, s: String) -> Id {
        self.items.push(s);
        Id(self.items.len() - 1)
    }
    fn get(&self, id: Id) -> &str {
        &self.items[id.0]
    }
}
```

这类 API：用户几乎不接触 lifetime，但还能零拷贝读取。

---

## 3) 你写项目时的“决策口诀”

你每次要设计函数签名时，按这个选：

1. **返回值要长期保存？** → 返回 owned（`String/Vec/Struct`）或 `Arc`
2. **只临时用一下引用，不想让它逃逸？** → 闭包式 `with_xxx(|x| ...)`
3. **struct 内部有数据，方法想返回切片？** → `&self -> &T`（规则 3 会帮你）
4. **要零拷贝但有时要改？** → `Cow`
5. **资源池/管理器/对象图？** → `Id/Handle` 模式
