# Brainfuck 解释器

用 Rust 编写的 Brainfuck 解释器。

## 简介

该程序读取 Brainfuck 源文件,将其解析为抽象语法树(AST),然后在 30,000 个字节单元的磁带上执行。非命令字符会被当作注释忽略,因此你可以随意在程序中添加空白和 `//` 风格的注释。

## 构建

```sh
cargo build --release
```

编译出的可执行文件位于 `target/release/interpreter`。

## 使用方法

```sh
./target/release/interpreter <file.bf>
```

只有**第一个**参数会被当作 Brainfuck 源文件。多余参数会被忽略并打印警告:

```
$ ./target/release/interpreter hello.bf extra.bf
ignoring extra argument: extra.bf
Hello World!
```

## 语言

该解释器支持标准的 8 条 Brainfuck 指令,其余字符一律忽略。

| 指令 | 名称     | 作用                                 |
| ---- | -------- | ------------------------------------ |
| `>`  | 右移     | 指针向右移动一格                     |
| `<`  | 左移     | 指针向左移动一格                     |
| `+`  | 自增     | 当前单元的值加一(循环溢出)           |
| `-`  | 自减     | 当前单元的值减一(循环溢出)           |
| `.`  | 输出     | 将当前单元作为一个字节写到 stdout    |
| `,`  | 输入     | 从 stdin 读一个字节存入当前单元      |
| `[`  | 循环开始 | 若当前单元为 0,跳转到匹配的 `]` 之后 |
| `]`  | 循环结束 | 若当前单元不为 0,跳转回匹配的 `[`    |

## 示例

### 1. Hello World

文件 `hello.bf`:

```brainfuck
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
```

运行:

```sh
$ ./target/release/interpreter hello.bf
Hello World!
```

### 2. 一个简单的循环

这个程序计算 5 × 13 并输出结果(`5 * 13 = 65`,即字符 `'A'`):

```brainfuck
+++++
[
  >+++++++++++++
  <-
]
>.
```

逐步说明:

1. `+++++` 将单元 0 设为 5。
2. 只要单元 0 不为 0,循环就一直执行:给单元 1 加 13,再把单元 0 减 1。
3. 循环 5 次后,单元 1 的值是 `5 * 13 = 65`。
4. `>.` 把指针移到单元 1 并打印,得到 `'A'`。

```sh
$ ./target/release/interpreter mult.bf
A
```

### 3. 回显(cat 程序)

把 stdin 原样复制到 stdout,直到读到字节 0:

```brainfuck
,[.,]
```

```sh
$ printf 'hello' | ./target/release/interpreter cat.bf
hello
```

## 特性

- **基于 AST 执行** — 循环在解析阶段被编译成嵌套的 `Loop` 节点,执行时不会重新扫描源码文本,而是一次直接的树遍历。
- **指令合并** — 连续的 `+`/`-` 会被合并为单个 `AddValue(n)` 节点,连续的 `>`/`<` 合并为单个 `MovePtr(n)` 节点。密集的数值循环因此比逐字符解释快得多。
- **溢出算术** — 单元值以 `u8` 模 256 循环,`255 + 1 == 0`。
- **括号配对检查** — 不匹配的 `[` 或 `]` 在解析阶段就能被发现并报错,而不是直接崩溃。
- **容忍空白与注释** — 所有非命令字符都会被静默跳过。

## 内存模型

- 磁带大小:**30,000 个单元**,在 `main()` 中固定。
- 单元类型:`u8`,值在 256 处循环。
- 指针从单元 0 开始,通过 `MovePtr` 移动。
- 指针越界(负数或 ≥ 磁带大小)会直接 panic 并打印信息。

## 错误处理

| 情况             | 行为                                                    |
| ---------------- | ------------------------------------------------------- |
| 文件无法读取     | stderr 输出 `Error reading <file>: <reason>`            |
| 多余的 `]`       | stderr 输出 `Error parsing <file>: Unmatched ']' found` |
| 多余的 `[`       | stderr 输出 `Error parsing <file>: Unmatched '[' found` |
| 指针越界         | Panic:`Pointer out of bounds: <idx>`                    |
| 多余的命令行参数 | 打印警告并忽略                                          |

## 实现细节

所有代码都在 `src/main.rs` 中:

- `parser(contents) -> Result<Vec<Node>, String>` — 把源码解析为 `Vec<Node>`。一个 `stack` 维护所有未闭合的 `[` 块;`flush()` 把连续的加法/移动指令合并为单个节点;括号不匹配时返回错误。
- `Executor { memory: Vec<u8>, ptr: usize }` — 持有磁带和指针。`execute()` 递归遍历 AST,`Loop` 节点内部用 `while` 循环直到当前单元变为 0。
- `main()` — 创建 30,000 个单元的执行器,对每个命令行参数依次执行 `read_file` → `parser` → `execute`。

合并行为可以直接观察:对 `+++>---<`,解析器生成

```
AddValue(3), MovePtr(1), AddValue(-3), MovePtr(-1)
```

而不是 8 个独立的单步节点。
