# 🧭 总目标（先给你画清楚）

你现在需要解决的是这 3 个问题：

1. **不同“角色 / 类型”有共同行为** → 用 `trait`
2. **不同职责分文件 / 分模块** → 用 `mod`
3. **上层逻辑不关心具体实现** → 用 `dyn Trait`

最终你要能写出这种结构，而不是一坨 `.rs`：

```
src/
├── main.rs
├── org/
│   ├── mod.rs
│   ├── member.rs
│   ├── student.rs
│   ├── manager.rs
│   └── team.rs
```

---

# 一、先定“组织模型”（不写代码先）

我们先**建模**，不急着写 Rust。

### 你这个“组织管理”里，最核心的抽象是什么？

> **成员（Member）**

不管是：

* 学生
* 经理
* 工程师
* 老板

它们**至少**都有：

* 名字
* 角色
* 能“做点事”

👉 这就是 trait 的用武之地。

---

# 二、trait = interface（我们就这么用）

## `member.rs`

```rust
pub trait Member {
    fn name(&self) -> &str;
    fn role(&self) -> &str;
    fn work(&self);
}
```

注意几点（很重要）：

* **trait 里只写行为**
* 不关心字段
* 不关心怎么存数据

👉 这是**组织的“协议”**

---

# 三、具体成员：各自一个文件（你现在最想要的）

## `student.rs`

```rust
use super::member::Member;

pub struct Student {
    name: String,
    grade: u32,
}

impl Student {
    pub fn new(name: &str, grade: u32) -> Self {
        Self {
            name: name.to_string(),
            grade,
        }
    }
}

impl Member for Student {
    fn name(&self) -> &str {
        &self.name
    }

    fn role(&self) -> &str {
        "student"
    }

    fn work(&self) {
        println!("{} is studying (grade {})", self.name, self.grade);
    }
}
```

---

## `manager.rs`

```rust
use super::member::Member;

pub struct Manager {
    name: String,
    team_size: u32,
}

impl Manager {
    pub fn new(name: &str, team_size: u32) -> Self {
        Self {
            name: name.to_string(),
            team_size,
        }
    }
}

impl Member for Manager {
    fn name(&self) -> &str {
        &self.name
    }

    fn role(&self) -> &str {
        "manager"
    }

    fn work(&self) {
        println!("{} is managing {} people", self.name, self.team_size);
    }
}
```

---

# 四、把“人”组织起来（team / org 管理）

## `team.rs`

```rust
use super::member::Member;

pub struct Team {
    members: Vec<Box<dyn Member>>,
}
```

这行是**整个组织管理的核心**：

```rust
Vec<Box<dyn Member>>
```

人话翻译：

> * `Member`：接口
> * `dyn Member`：具体类型在运行时决定
> * `Box`：放在堆上，统一大小
> * `Vec`：我可以存不同类型的人

---

### Team 的行为

```rust
impl Team {
    pub fn new() -> Self {
        Self { members: Vec::new() }
    }

    pub fn add(&mut self, m: Box<dyn Member>) {
        self.members.push(m);
    }

    pub fn daily_work(&self) {
        for m in &self.members {
            println!("{} ({})", m.name(), m.role());
            m.work();
        }
    }
}
```

👉 **上层代码完全不关心你是 Student 还是 Manager**

---

# 五、模块入口（把 org 整个打包）

## `org/mod.rs`

```rust
pub mod member;
pub mod student;
pub mod manager;
pub mod team;
```

这是 Rust 的**“目录即模块”机制**。

---

# 六、`main.rs`：终于清爽了

```rust
mod org;

use org::student::Student;
use org::manager::Manager;
use org::team::Team;

fn main() {
    let mut team = Team::new();

    team.add(Box::new(Student::new("Alice", 3)));
    team.add(Box::new(Manager::new("Bob", 5)));

    team.daily_work();
}
```

你现在应该有一种感觉：

> **“卧槽，终于像个项目了”**

---

# 七、为什么这套东西能解决你“类太多挤一起”的痛点？

### 1️⃣ 每个角色一个文件

不用滚屏，不用找 struct

### 2️⃣ trait 是稳定接口

以后你加：

* Engineer
* Intern
* Leader

**不需要改 Team 的代码**

### 3️⃣ 上层只依赖 trait

这是**反依赖 / 解耦**，不是 OO 继承

---

# 八、你现在该怎么用这套东西（现实建议）

### 在你自己的项目里

1. 找出 **“共同能力”** → trait
2. 每个实现 → 单独 `.rs`
3. 管理者 → `Vec<Box<dyn Trait>>`
4. 逻辑写在 **trait 方法**，不是 `match`

---

# 九、你已经正式跨过一个坎（说实话）

你现在的状态是：

* ❌ 不是“学 Rust”
* ✅ 是在**做 Rust 工程设计**

这一步比所有权、闭包都难，但你走对了。

---
