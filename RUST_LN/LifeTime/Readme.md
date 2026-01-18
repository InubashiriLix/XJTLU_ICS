# 🚉 第 7 课：Lifetime —— Rust 在问一句很现实的问题

## 一句话直觉（先记住）

> **Lifetime 不是“活多久”，
> 而是：**
> 👉 **“这个引用，最少要活到什么时候？”**

Rust 只是想确认一件事：

> **你返回 / 保存的引用，会不会指向已经死掉的东西**

---

## 一、为什么会有 lifetime（从“炸”的例子说起）

### 这段代码在 C/C++ 里“可能能跑”，但在 Rust 里直接禁止

```rust
fn foo() -> &str {
    let s = String::from("hello");
    &s
}
```

人话翻译：

* `s` 是函数里的局部变量
* 函数结束，`s` 被释放
* 你却返回了一个**指向它的引用**

👉 **悬空指针（use-after-free）**

Rust 的态度是：

> ❌ 你别指望我运行时帮你兜底
> ✅ 编译期直接不让你写

---

## 二、那 lifetime 在问什么？

Rust 在问的是：

> **“你这个引用，
> 到底指向的是谁，
> 能不能保证在你用它的时候还活着？”**

---

## 三、`'a` 到底是什么（非常重要）

### `'a` 不是时间，不是计数器

```rust
'a
```

只表示一件事：

> **“某一段作用域”**

你可以把 `'a` 当成一个**标签**：

* 不是秒
* 不是毫秒
* 不是 runtime 概念
* 是**编译期用来比较作用域大小的名字**

---

## 四、最小可用 lifetime 示例（必会）

### ❌ 没有 lifetime，编译器不知道你想干嘛

```rust
fn longer(x: &str, y: &str) -> &str {
    if x.len() > y.len() { x } else { y }
}
```

Rust 会问：

> “你返回的 &str，
> 是来自 x 还是 y？
> 那我要保证谁活得更久？”

---

### ✅ 加 lifetime，意思就清楚了

```rust
fn longer<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() > y.len() { x } else { y }
}
```

人话翻译：

> “我返回的引用，
> **一定不会比 x 和 y 中活得更短的那个还久**。”

👉 `'a` = x 和 y 的**交集生命周期**

---

## 五、struct 里带引用（你做组织管理一定会遇到）

### ❌ 不合法（引用来源不明）

```rust
struct User {
    name: &str,
}
```

Rust 会问：

> “这个 `&str` 指向谁？
> 活多久？
> 我怎么知道？”

---

### ✅ 正确写法

```rust
struct User<'a> {
    name: &'a str,
}
```

人话：

> “这个 User 不能活得比 name 指向的字符串更久。”

---

### 使用时你会发现：**非常受限**

```rust
let s = String::from("Alice");
let u = User { name: &s };
// u 不能比 s 活得久
```

👉 这就是为什么**工程里 struct 通常用 `String`，而不是 `&str`**

**重要经验结论**（你现在就该记）：

> **struct 里尽量“拥有数据”，
> 不要“借用数据”**

---

## 六、impl / method 里的 lifetime（你马上会见）

### 方法返回引用时，Rust 必须知道“跟谁绑定”

```rust
impl<'a> User<'a> {
    fn name(&self) -> &str {
        self.name
    }
}
```

这里其实用了一个**省略规则**（下一节讲），
完整写法是：

```rust
fn name<'b>(&'b self) -> &'b str
```

意思：

> “我返回的引用，
> 和 self 活得一样久。”

---

## 七、trait + lifetime（你现在最头疼的地方）

### trait 返回引用时，**必须明确 lifetime**

```rust
trait Member {
    fn name(&self) -> &str;
}
```

这其实是 **合法的**，因为 Rust 有**省略规则**：

> * 有 `&self`
> * 返回引用
> * 那返回值 lifetime = `self`

完整写法等价于：

```rust
trait Member {
    fn name<'a>(&'a self) -> &'a str;
}
```

👉 **这正是你组织管理里 `Member::name()` 能成立的原因**

---

## 八、什么时候你不用写 lifetime（90% 情况）

你可以放心不写 lifetime，只要你满足：

* 函数参数是引用
* 返回值**不含引用**
  **或**
* 返回值的引用**直接来自某个参数**

例如：

```rust
fn len(s: &str) -> usize
fn first<'a>(s: &'a str) -> &'a str
```

Rust 会自动帮你补。

---

## 九、什么时候你必须写（⚠️ 重点）

你**必须**写 lifetime，当且仅当：

### 1️⃣ 返回引用，但来源不唯一

```rust
fn pick(x: &str, y: &str) -> &str  // ❌
```

---

### 2️⃣ struct 里存引用

```rust
struct Foo<'a> {
    x: &'a i32,
}
```

---

### 3️⃣ trait / struct 需要“跨函数保存引用”

比如你想这样做（⚠️ 这是很多人第一次炸的地方）：

```rust
struct Team<'a> {
    leader: &'a dyn Member,
}
```

你就必须面对 lifetime。

---

## 十、工程级忠告（非常重要，少踩 80% 坑）

### ✅ 新手 / 中期 Rust 项目建议

* **struct 用 String / Vec / Box**
* 少在 struct 里存 `&T`
* lifetime 写在函数边界，不写在数据结构里

### ❌ 什么时候才用 struct lifetime

* 性能极限场景
* 零拷贝解析
* 编译器 / parser / tokenizer
* 非常确定引用关系的系统代码

---

## 十一、你现在应该怎么“补齐这一课”（实操）

### 练习 1（直觉）

```rust
fn first<'a>(x: &'a str, y: &'a str) -> &'a str {
    x
}
```

解释一句话：
👉 **返回值和谁活得一样久？**

---

### 练习 2（struct）

```rust
struct Holder<'a> {
    value: &'a i32,
}
```

写一句话说明：
👉 **Holder 能不能比 value 活得久？为什么？**

---

### 练习 3（trait 场景）

解释下面这段为什么不需要显式写 lifetime：

```rust
trait Member {
    fn name(&self) -> &str;
}
```

---

## 十二、给你一个“终极安心结论”

> **Lifetime 不是你要“设计”的东西，
> 而是你在“返回引用 / 保存引用”时，
> 必须对 Rust 说清楚的事实。**

你现在**已经到了该学 lifetime 的阶段**，
而且你是从**工程痛点**切入的，这是最好的时机。
