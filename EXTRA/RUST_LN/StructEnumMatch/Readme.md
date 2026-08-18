# 🚀 第 3 课：结构体 / 枚举 / match（Rust 精华）

**目标**

* 能写 struct 和 enum
* 能用 match 完全替代 C++ 的 switch / if 链
* 能自然处理不同类型数据
* 完全用你现在的“所有权 + 借用”直觉

---

## 1️⃣ struct（结构体）——把东西放到一起

C++ 的 class/struct，Rust 简化版：

```rust
struct Point {
    x: i32,
    y: i32,
}

fn main() {
    let mut p = Point { x: 1, y: 2 };
    println!("({}, {})", p.x, p.y);

    p.x = 10; // ✅ 可变
}
```

特点：

* 变量默认不可变，要 `mut` 才能改
* 没有方法？也可以写（下一课讲 trait / impl）
* **每个 struct 自带“谁是主人”规则”**，默认 move

---

## 2️⃣ enum（枚举）——不同类型的一堆可能性

Rust 的 enum 很像“C++ union + type tag”，但安全得多：

```rust
enum Shape {
    Circle(f64),        // 半径
    Rectangle(f64, f64) // 宽, 高
}

fn main() {
    let s1 = Shape::Circle(3.0);
    let s2 = Shape::Rectangle(4.0, 5.0);
}
```

特点：

* 每个值“自带类型信息”
* 访问时必须用 `match` 或 `if let`，Rust 保证安全

---

## 3️⃣ match（神奇的万能分支）

```rust
fn area(shape: &Shape) -> f64 {
    match shape {
        Shape::Circle(r) => 3.14 * r * r,
        Shape::Rectangle(w, h) => w * h,
    }
}
```

特点：

* **穷尽检查**：Rust 编译器会确保你没漏一个可能的 enum 分支
* 替代 C++ 的 switch + 动态 cast + if 链

---

## 4️⃣ match + 所有权结合

```rust
enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
}

fn process(msg: Message) {
    match msg {
        Message::Quit => println!("Quit"),
        Message::Move { x, y } => println!("Move to ({}, {})", x, y),
        Message::Write(s) => println!("Write: {}", s),
    }
}

fn main() {
    let m = Message::Write(String::from("hello"));
    process(m);
    // println!("{}", m); // ❌ m 已 move
}
```

✅ 结合你之前学的借用规则

* enum 携带数据
* match 解包
* move 自动触发

这就是 Rust 的**安全多态**。

---

## 5️⃣ 小练习（必须写）

### 练习 1：Point 变形

```rust
struct Point {
    x: i32,
    y: i32,
}

fn move_right(p: &mut Point, dx: i32) {
    // 改变 p.x
}

fn main() {
    let mut p = Point { x: 0, y: 0 };
    move_right(&mut p, 5);
    println!("p = ({}, {})", p.x, p.y); // 5, 0
}
```

---

### 练习 2：Shape 面积计算

```rust
enum Shape {
    Circle(f64),
    Rectangle(f64, f64),
}

fn area(shape: &Shape) -> f64 {
    // 用 match
}

fn main() {
    let s1 = Shape::Circle(3.0);
    let s2 = Shape::Rectangle(4.0, 5.0);
    println!("area1 = {}", area(&s1));
    println!("area2 = {}", area(&s2));
}
```

---

### 练习 3（挑战 + 所有权）

```rust
enum Message {
    Quit,
    Write(String),
}

fn process(msg: Message) {
    // match 处理
}

fn main() {
    let m = Message::Write(String::from("hi"));
    process(m);
    // println!("{:?}", m); // ❌ 为什么？
}
```

---

💡 **通关秘诀**：

* **struct** = 数据打包
* **enum** = 不同类型 + 数据打包
* **match** = “分支 + 解包 + 安全检查”
* **所有权规则** = Rust 的安全底盘，**不管 C++、不管 GC**

---
