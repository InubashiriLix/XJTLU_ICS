# 🌳 Rust 智能指针（超详细版 + 可运行例子 + 实战）

## 0️⃣ 先立“选型思维”（你以后靠这个秒选）

你每次遇到指针/共享/并发问题，先问三句：

1. **谁拥有它？**（一个人 / 多个人）
2. **会不会修改它？**（不改 / 要改）
3. **在不在线程里？**（单线程 / 多线程）

然后再选：

* 单一所有者 → `Box<T>`
* 多所有者（单线程）→ `Rc<T>`
* 多所有者（多线程）→ `Arc<T>`
* 需要“共享 + 可变”（单线程）→ `Rc<RefCell<T>>`
* 需要“共享 + 可变”（多线程）→ `Arc<Mutex<T>>` 或 `Arc<RwLock<T>>`
* 需要避免循环引用 → `Weak<T>`

---

# 1️⃣ `Box<T>`：堆上放东西 + 统一大小（trait object / 递归结构必备）

## 1.1 堆上分配（最简单）

```rust
fn main() {
    let b = Box::new(5);
    println!("b = {}", b);
}
```

直觉：`Box<T>` 就像 “`T*` 但 Rust 管理释放”。

---

## 1.2 trait object：组织管理 `Vec<Box<dyn Trait>>`（你会最常用）

### 为什么必须 Box？

`dyn Trait` 大小不确定，`Vec` 需要元素大小统一，所以用 Box 把它变成固定大小指针。

```rust
trait Member {
    fn name(&self) -> &str;
    fn work(&self);
}

struct Student { name: String }
impl Member for Student {
    fn name(&self) -> &str { &self.name }
    fn work(&self) { println!("{} studies", self.name); }
}

struct Manager { name: String }
impl Member for Manager {
    fn name(&self) -> &str { &self.name }
    fn work(&self) { println!("{} manages", self.name); }
}

fn main() {
    let mut team: Vec<Box<dyn Member>> = Vec::new();
    team.push(Box::new(Student { name: "Alice".into() }));
    team.push(Box::new(Manager { name: "Bob".into() }));

    for m in &team {
        println!("member: {}", m.name());
        m.work();
    }
}
```

这就是你之前说的“interface”，**trait object + Box** 就是 Rust 的“多态集合”。

---

## 1.3 递归结构：没有 Box 编译器算不出大小

```rust
enum List {
    Cons(i32, Box<List>),
    Nil,
}

fn main() {
    let l = List::Cons(1, Box::new(List::Cons(2, Box::new(List::Nil))));
}
```

---

# 2️⃣ `Rc<T>`：单线程共享所有权（引用计数）

## 2.1 多个 owner（共享“拥有权”）

```rust
use std::rc::Rc;

fn main() {
    let s = Rc::new(String::from("hello"));

    let a = Rc::clone(&s);
    let b = Rc::clone(&s);

    println!("s = {}", s);
    println!("a = {}", a);
    println!("b = {}", b);
    println!("strong_count = {}", Rc::strong_count(&s));
}
```

你可以把 `Rc::strong_count` 理解为：
“现在有多少个人拿着这东西的钥匙”。

---

## 2.2 Rc 默认**不能改**（只共享读）

因为共享可变会引入数据竞争，所以 Rust 默认让 Rc 只读。

要改？下一节 `RefCell`。

---

# 3️⃣ `RefCell<T>`：运行时借用检查（单线程“内部可变性”）

你要把它理解成：

> 编译器：我证明不了你安全
> 你：我保证安全
> Rust：行，但我运行时盯着你，违规就 panic

## 3.1 基础用法

```rust
use std::cell::RefCell;

fn main() {
    let x = RefCell::new(5);

    *x.borrow_mut() += 1;
    println!("x = {}", x.borrow());
}
```

### ⚠️ 它怎么炸（这就是它的风险）

同一时间借了只读，又借可写：

```rust
use std::cell::RefCell;

fn main() {
    let x = RefCell::new(5);

    let _r = x.borrow();        // 只读借用
    let _w = x.borrow_mut();    // 💥 运行时 panic：already borrowed
}
```

---

## 3.2 `Rc<RefCell<T>>`：单线程共享 + 可变（超常用组合）

这组合很像“很多地方都能拿到同一个对象，并且能改”。

```rust
use std::rc::Rc;
use std::cell::RefCell;

#[derive(Debug)]
struct Counter { v: i32 }

fn main() {
    let c = Rc::new(RefCell::new(Counter { v: 0 }));

    let c1 = Rc::clone(&c);
    let c2 = Rc::clone(&c);

    c1.borrow_mut().v += 10;
    c2.borrow_mut().v += 5;

    println!("counter = {:?}", c.borrow()); // 15
}
```

---

# 4️⃣ `Arc<T>`：多线程共享所有权（Rc 的线程安全版）

## 4.1 跨线程共享只读数据

```rust
use std::sync::Arc;
use std::thread;

fn main() {
    let s = Arc::new(String::from("hello"));

    let t1 = {
        let s = Arc::clone(&s);
        thread::spawn(move || println!("t1: {}", s))
    };

    let t2 = {
        let s = Arc::clone(&s);
        thread::spawn(move || println!("t2: {}", s))
    };

    t1.join().unwrap();
    t2.join().unwrap();
}
```

---

# 5️⃣ `Mutex<T>`：多线程共享 + 可变（最常见）

## 5.1 `Arc<Mutex<T>>`：多线程计数器（经典实战）

```rust
use std::sync::{Arc, Mutex};
use std::thread;

fn main() {
    let counter = Arc::new(Mutex::new(0));

    let mut handles = vec![];

    for _ in 0..10 {
        let c = Arc::clone(&counter);
        handles.push(thread::spawn(move || {
            let mut guard = c.lock().unwrap();
            *guard += 1;
        }));
    }

    for h in handles { h.join().unwrap(); }

    println!("counter = {}", *counter.lock().unwrap()); // 10
}
```

**关键直觉**：
`lock()` 返回 guard，guard 掉出作用域自动解锁。

---

# 6️⃣ `RwLock<T>`：读多写少（缓存/配置必备）

```rust
use std::sync::{Arc, RwLock};
use std::thread;

fn main() {
    let data = Arc::new(RwLock::new(vec![1,2,3]));

    let r1 = {
        let data = Arc::clone(&data);
        thread::spawn(move || {
            let g = data.read().unwrap();
            println!("reader1: {:?}", *g);
        })
    };

    let w = {
        let data = Arc::clone(&data);
        thread::spawn(move || {
            let mut g = data.write().unwrap();
            g.push(4);
        })
    };

    r1.join().unwrap();
    w.join().unwrap();
    println!("final = {:?}", *data.read().unwrap());
}
```

---

# 7️⃣ `Weak<T>`：解决循环引用（树结构“父指针”）

如果你用 `Rc` 做树：

* `parent` 拿 `Rc` 指向孩子
* `child` 又拿 `Rc` 指回 parent
  → 引用计数永远不为 0，内存泄漏

### 正确结构：孩子强引用父亲？不，孩子用 Weak 指父亲

```rust
use std::rc::{Rc, Weak};
use std::cell::RefCell;

#[derive(Debug)]
struct Node {
    name: String,
    parent: RefCell<Weak<Node>>,
    children: RefCell<Vec<Rc<Node>>>,
}

impl Node {
    fn new(name: &str) -> Rc<Node> {
        Rc::new(Node {
            name: name.into(),
            parent: RefCell::new(Weak::new()),
            children: RefCell::new(vec![]),
        })
    }

    fn add_child(parent: &Rc<Node>, child: Rc<Node>) {
        *child.parent.borrow_mut() = Rc::downgrade(parent); // Weak 指向 parent
        parent.children.borrow_mut().push(child);
    }
}

fn main() {
    let root = Node::new("root");
    let child = Node::new("child");

    Node::add_child(&root, Rc::clone(&child));

    println!("root strong = {}", Rc::strong_count(&root));
    println!("child strong = {}", Rc::strong_count(&child));

    // 从 child 找 parent
    if let Some(p) = child.parent.borrow().upgrade() {
        println!("child's parent = {}", p.name);
    }
}
```

---

# ✅ 实战：组织管理系统（单线程版 + 可选升级多线程）

你说要“实战”，我们就做一个你能扩展的 mini 架构：

目标：

* 用 trait 表示角色 interface
* 用 `Vec<Box<dyn Member>>` 管理不同角色
* 用 `Rc<RefCell<_>>` 做“共享可变成员”（比如成员状态可变）
* 用 `Weak` 做“成员属于哪个团队”的反向引用（避免循环）

---

## 实战 1：最简团队（Box + dyn Trait）

```rust
trait Member {
    fn name(&self) -> &str;
    fn work(&self);
}

struct Student { name: String }
impl Member for Student {
    fn name(&self) -> &str { &self.name }
    fn work(&self) { println!("{} studies", self.name); }
}

struct Team {
    members: Vec<Box<dyn Member>>,
}

impl Team {
    fn new() -> Self { Self { members: vec![] } }
    fn add(&mut self, m: Box<dyn Member>) { self.members.push(m); }
    fn run(&self) {
        for m in &self.members { m.work(); }
    }
}

fn main() {
    let mut t = Team::new();
    t.add(Box::new(Student { name: "Alice".into() }));
    t.run();
}
```

你现在已经能“按文件分模块”把它拆开了。

---

## 实战 2：成员有状态（Rc<RefCell<T>>）

我们让成员有一个 `status`，可以被多个地方修改。

```rust
use std::rc::Rc;
use std::cell::RefCell;

#[derive(Debug)]
struct PersonState {
    status: String,
}

trait Member {
    fn name(&self) -> &str;
    fn work(&self);
    fn state(&self) -> Rc<RefCell<PersonState>>;
}

struct Student {
    name: String,
    st: Rc<RefCell<PersonState>>,
}

impl Student {
    fn new(name: &str) -> Self {
        Self {
            name: name.into(),
            st: Rc::new(RefCell::new(PersonState { status: "idle".into() })),
        }
    }
}

impl Member for Student {
    fn name(&self) -> &str { &self.name }

    fn work(&self) {
        self.st.borrow_mut().status = "studying".into();
        println!("{} is working, state={:?}", self.name, self.st.borrow());
    }

    fn state(&self) -> Rc<RefCell<PersonState>> {
        Rc::clone(&self.st)
    }
}

fn main() {
    let s = Student::new("Alice");
    let shared_state = s.state();     // 共享出去
    s.work();
    shared_state.borrow_mut().status = "tired".into();
    println!("after edit: {:?}", shared_state.borrow());
}
```

这就是 `Rc<RefCell<T>>` 的真实工程用法：
**“共享 + 可变 + 单线程”**。

---

## 实战 3（升级）：多线程调度（Arc<Mutex<T>>）

如果你要在多线程里共享成员状态：

* `Rc` 换 `Arc`
* `RefCell` 换 `Mutex` 或 `RwLock`

你会得到：

```rust
use std::sync::{Arc, Mutex};

type SharedState = Arc<Mutex<PersonState>>;
```

---

# 🧠 最后给你一个“反滥用指南”（很重要）

* 能 `&mut` 解决的，别上 `RefCell/Mutex`
* 单线程共享可变 → `Rc<RefCell<T>>`
* 多线程共享可变 → `Arc<Mutex<T>>`（读多写少 → `RwLock`）
* 出现循环引用就想到 `Weak`

---

如果你想，我可以下一步直接帮你把“组织管理系统”整理成一个**真正的 crate 文件结构**（你刚说过挤在一个文件里难受）：

* `src/org/member.rs`
* `src/org/team.rs`
* `src/org/roles/student.rs`
* `src/org/roles/manager.rs`
* `src/main.rs`

并且给你一个“加新角色只需要新增一个文件”的模板。

你要我做 **单线程版（Rc/RefCell）** 还是 **多线程版（Arc/Mutex）**？
