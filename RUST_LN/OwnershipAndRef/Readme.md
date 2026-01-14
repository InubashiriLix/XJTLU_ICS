# 🧠 第 2 课：所有权 & 借用（Rust 的灵魂）

这节课是**分水岭**：

* 过了：Rust 开始“顺手”
* 不过：天天和编译器对骂

我会用 **最少规则 + 强直觉** 讲，不搞学术。

---

## 1️⃣ 一句话规则（先背）

> **Rust 中，每个值只能有一个“主人”**

```rust
let s = String::from("hello");
```

此时：

* `"hello"` 的主人是 `s`

---

## 2️⃣ 所有权移动（90% 新手崩溃点）

```rust
let s1 = String::from("hello");
let s2 = s1;

println!("{}", s1); // ❌ 编译错误
```

为什么？

* `String` 在堆上
* Rust 不允许**隐式拷贝**
* 所以：**所有权从 `s1` 移动到 `s2`**

`s1` 直接作废（编译期报错）

👉 这不是小气，是为了：

* 没有 double free
* 没有悬空指针
* 没有 GC

---

## 3️⃣ Copy 类型不会出事

```rust
let a = 5;
let b = a;

println!("{}", a); // ✅
```

因为：

* `i32` 是 `Copy`
* 栈上
* 直接复制值

**判断口诀：**

> 小、定长、栈上的 → Copy
> 堆上的 → Move

---

## 4️⃣ 函数参数 = 所有权传递

```rust
fn take(s: String) {
    println!("{}", s);
}

fn main() {
    let s = String::from("hi");
    take(s);
    println!("{}", s); // ❌
}
```

`s` 被 move 给了 `take`

---

## 5️⃣ 借用（&）—— Rust 的救命绳

```rust
fn take_ref(s: &String) {
    println!("{}", s);
}

fn main() {
    let s = String::from("hi");
    take_ref(&s);
    println!("{}", s); // ✅
}
```

规则开始变得重要了：

---

## 6️⃣ 借用三大铁律（**一定要记**）

### 铁律 1

> 同一时间，**可以有多个不可变借用**

```rust
let r1 = &s;
let r2 = &s;
```

### 铁律 2

> 同一时间，**只能有一个可变借用**

```rust
let r = &mut s;
```

### 铁律 3（最重要）

> **可变借用 和 不可变借用 不能同时存在**

```rust
let r1 = &s;
let r2 = &mut s; // ❌
```

---

# ✍️ 第 2 课练习（关键）

### 练习 1（必做）

写函数：

```rust
fn string_len(s: &String) -> usize
```

* 不能获取所有权
* 返回字符串长度

---

### 练习 2（必炸一次）

写函数：

```rust
fn append_world(s: &mut String)
```

* 给字符串加 `" world"`

在 `main` 中正确调用它

---

### 练习 3（思考题）

下面代码哪里错？**为什么？**

```rust
let mut s = String::from("hello");

let r1 = &s;
let r2 = &mut s;

println!("{}", r1);
```

---

## 🎯 通关标准（严格）

你需要：

* 写出 **能编译通过** 的练习 1 & 2
* 能**用自己的话解释**练习 3 的错误
