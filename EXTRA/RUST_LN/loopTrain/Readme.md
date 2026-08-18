# 🚀 第 1 课：Rust 能跑、能写、不炸

### 🎯 本课目标

你学完这节，能：

* 用 cargo 建项目
* 看懂 main.rs
* 写基本逻辑（变量 / if / loop / 函数）
* 不被语法吓到

---

## 1️⃣ 创建项目（就一次）

```bash
cargo new rust_fast
cd rust_fast
cargo run
```

你会看到：

```text
Hello, world!
```

项目结构只记这三个：

```
rust_fast/
 ├─ Cargo.toml   # 配置（类似 package.json / CMakeLists）
 └─ src/
     └─ main.rs  # 入口
```

---

## 2️⃣ main.rs 最小理解

```rust
fn main() {
    println!("Hello, world!");
}
```

* `fn`：函数
* `main`：入口
* `!`：宏（先别管）
* `;`：**语句结束必须有**

---

## 3️⃣ 变量（Rust 开始搞事的地方）

```rust
let x = 5;
let mut y = 10;

y = y + 1;
```

重点就一句话：

> **默认不可变，想改必须 `mut`**

这不是恶心你，是为了：

* 防 bug
* 方便并发
* 编译器更聪明

---

## 4️⃣ 基本类型（不用背）

```rust
let a: i32 = 10;
let b: f64 = 3.14;
let c: bool = true;
let d: char = 'A';
```

常用就：

* `i32`
* `f64`
* `bool`
* `char`
* `String`

---

## 5️⃣ if / loop / while / for

### if（没括号）

```rust
if x > 5 {
    println!("big");
} else {
    println!("small");
}
```

### loop（死循环）

```rust
let mut i = 0;
loop {
    i += 1;
    if i == 3 {
        break;
    }
}
```

### while

```rust
let mut n = 3;
while n > 0 {
    println!("{}", n);
    n -= 1;
}
```

### for（**强烈推荐**）

```rust
for i in 0..5 {
    println!("{}", i);
}
```

---

## 6️⃣ 函数（⚠️ 注意返回值）

```rust
fn add(a: i32, b: i32) -> i32 {
    a + b   // 没有分号 = 返回值
}
```

如果你写成：

```rust
a + b;
```

👉 **返回 `()`（空）**，直接报错
这是 Rust 很重要的“表达式语言”特性

---

## 7️⃣ println! 常用姿势

```rust
let x = 10;
println!("x = {}", x);
println!("x + 1 = {}", x + 1);
```

---

# ✍️ 本课练习（必须写）

### 练习 1（热身）

写一个函数：

```rust
fn is_even(n: i32) -> bool
```

* 偶数返回 `true`
* 奇数返回 `false`
* 在 `main` 里测试

---

### 练习 2（循环）

在 `main` 中：

* 用 `for`
* 打印 `1..=10`
* 只打印 **偶数**

---

### 练习 3（小思考）

写一个函数：

```rust
fn sum_to(n: i32) -> i32
```

返回 `1 + 2 + ... + n`

---

## ❗ 本课你不需要懂的东西

（以后会讲，现在别纠结）

* 所有权 / 借用
* 生命周期 `'a`
* trait
* async
* unsafe
* 宏怎么写
