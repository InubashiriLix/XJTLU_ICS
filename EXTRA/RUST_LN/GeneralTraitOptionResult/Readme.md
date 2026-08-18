## 第 4 课练习（写完直接升段位）

### 练习 1：给 Point 加方法（impl）

目标：你能写“像 class 的 method”

```rust
#[derive(Clone, Copy, Debug, PartialEq, Eq)]
struct Point { x: i32, y: i32 }

impl Point {
    fn new(x: i32, y: i32) -> Self { /* ... */ }
    fn move_by(&mut self, dx: i32, dy: i32) { /* ... */ }
    fn distance2(&self, other: Point) -> i32 { /* 返回平方距离 */ }
}
```

要求：

* `new` 返回 `Self`
* `move_by` 改自己（&mut self）
* `distance2` 不改自己（&self）

---

### 练习 2：Vec + enum：存一堆 Shape，算总面积

```rust
fn total_area(shapes: &[Shape]) -> f64 {
    // 遍历 Vec / slice
}
```

在 main 里：

* 放 3 个 shape 到 `Vec<Shape>`
* 打印总面积

提示：`for s in shapes { ... }`

---

### 练习 3：Option：矩形“可能无效”

写一个构造函数：当矩形宽或高为 0 时返回 None

```rust
fn make_rectangle(rt: Point, lb: Point) -> Option<Shape> {
    // 宽==0 或 高==0 => None
    // 否则 Some(Shape::Rectangle(...))
}
```

在 main 里测试：

* 一个合法矩形 -> Some
* 一个退化矩形 -> None
