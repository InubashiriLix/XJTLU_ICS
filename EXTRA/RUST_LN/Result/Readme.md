# 🚉 第 5 课：`Result` —— 不 throw，但更诚实

## 一句话结论（先记）

> **`Result` 不是异常，是“函数签名里写清楚：我可能失败”。**

```rust
Result<T, E>
```

* `Ok(T)`：成功，拿到结果
* `Err(E)`：失败，给出原因

---

## 1️⃣ 为什么 Rust 不用异常（throw）

因为异常有 3 个工程级大坑：

1. **看不到**：函数签名不说会不会炸
2. **不好组合**：嵌套 try/catch 读不下去
3. **控制流隐式**：你不知道哪行会跳走

Rust 的选择是：

> ❌ 让错误“飞”
> ✅ 让错误“写在类型里”

---

## 2️⃣ 最小可用示例（Result 的“白话原型”）

### 从字符串解析整数（match 版）

```rust
fn parse_i32(s: &str) -> Result<i32, String> {
    match s.parse::<i32>() {
        Ok(v) => Ok(v),
        Err(_) => Err(format!("cannot parse '{}'", s)),
    }
}
```

调用方**必须**处理：

```rust
match parse_i32("123") {
    Ok(v) => println!("value = {}", v),
    Err(e) => println!("error: {}", e),
}
```

👉 **编译器逼你面对失败，而不是假装失败不存在。**

---

## 3️⃣ `?` 运算符：把错误“往上扔”（但不是异常）

上面的 `match` 写法是**最原始、最直观的模型**。
Rust 提供了 `?`，只是为了**少写样板代码**。

### 等价的 `?` 版本

```rust
fn parse_i32(s: &str) -> Result<i32, String> {
    let v = s
        .parse::<i32>()
        .map_err(|e| format!("cannot parse '{}': {}", s, e))?;
    Ok(v)
}
```

### `?` 的人话解释

> “如果这里是 `Err`，
> 👉 立刻 `return Err(...)`
> 👉 否则继续用里面的值”

等价于你手写的：

```rust
match something {
    Ok(v) => v,
    Err(e) => return Err(e),
}
```

⚠️ **`?` 不是魔法，只是自动写 return。**

---

## 4️⃣ `map_err` + 闭包：错误“翻译器”

```rust
.map_err(|e| format!("cannot parse '{}': {}", s, e))
```

这段代码的人话含义是：

> “如果失败了，我不想要原来的错误类型，
> 我用一个闭包，把它转换成我想要的错误信息。”

关键点：

* `map_err` **只在 Err 分支执行**
* `|e| ...` 是一个**闭包（匿名函数）**
* 闭包可以**捕获外部变量**（这里是 `s`）

---

## 5️⃣ `Option` vs `Result`（一定要分清）

| 类型             | 什么时候用        |
| -------------- | ------------ |
| `Option<T>`    | **没有是正常情况**  |
| `Result<T, E>` | **失败需要解释原因** |

### 例子

```rust
fn find_user(id: i32) -> Option<User>        // 找不到很正常
fn read_file(path: &str) -> Result<String>  // 失败必须说原因
```

直觉判断法：

> **“没有值是不是一种正常状态？”**
>
> * 是 → `Option`
> * 否 → `Result`

---

## 6️⃣ 不要滥用 `unwrap()`（这是真正的雷）

```rust
let v = s.parse::<i32>().unwrap(); // 💣
```

这行代码的真实含义是：

> “如果失败，我选择**直接炸进程**。”

### 工程中可以接受的 unwrap 场景

* 测试代码
* demo
* **逻辑上 100% 不可能失败的地方**

其余情况，优先：

* `?`
* `match`
* `map_or_else`

---

## 7️⃣ 比 `match` 更短的 Result 处理写法（常用）

### `map_or_else`（同时处理 Ok / Err）

```rust
parse_i32("1234!")
    .map_or_else(
        |e| println!("error: {}", e),
        |v| println!("parsed ok: {}", v),
    );
```

### `unwrap_or_else`（给 Err 一个兜底）

```rust
let v = parse_i32("1234!")
    .unwrap_or_else(|e| {
        println!("{}", e);
        0
    });
```

---

## 8️⃣ 把错误“交给 main”是最 Rust 的 CLI 写法

```rust
fn main() -> Result<(), String> {
    let v = parse_i32("1234!")?;
    println!("parsed ok: {}", v);
    Ok(())
}
```

人话：

> “我不在这层处理错误，
> 出错就直接结束程序并报告。”

---

# ✍️ 本课实战练习（已验证你真的会）

## 练习 1：解析点字符串

```rust
fn parse_point(s: &str) -> Result<Point, String>
```

核心思路：

1. `split(',')`
2. 检查长度
3. `parse::<f64>()`
4. 用 `map_err + ?` 精确报错

---

## 练习 2：组合 Result

```rust
fn distance_from_str(a: &str, b: &str) -> Result<f64, String>
```

* 每一步都可能失败
* 用 `?` 串起来
* **错误自然向上传播**

---

## 练习 3（结论题）

> **为什么 `Result` 比异常更适合系统级语言？**

标准工程级答案可以总结为一句话：

> `Result` 把失败显式写进类型系统，
> 编译器能强制你处理错误；
> 相比异常的隐式跳转或 C 的模糊错误码，它更可预测、更安全、更易组合。

---

### 🧠 你现在真正“会”的是什么

到这一步，你已经真正理解了：

* `Result` 是**控制流的一部分**
* `?` 只是 `match + return` 的语法糖
* 闭包只是“临时小函数”
* Rust 不是要你写少代码，而是要你**写清楚责任**
