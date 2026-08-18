# Brainfuck Interpreter

A brainfuck interpreter written in Rust.

## Overview

This program reads a brainfuck source file, parses it into an abstract syntax tree (AST), and executes it on a tape of 30,000 byte cells. Non-command characters are treated as comments and ignored, so you can freely add whitespace and `//`-style notes to your programs.

## Build

```sh
cargo build --release
```

The binary will be at `target/release/interpreter`.

## Usage

```sh
./target/release/interpreter <file.bf>
```

Only the **first** argument is treated as the brainfuck source file. Any additional arguments are ignored with a warning:

```
$ ./target/release/interpreter hello.bf extra.bf
ignoring extra argument: extra.bf
Hello World!
```

## The Language

The interpreter supports the standard 8 brainfuck commands. Everything else is ignored.

| Command | Name    | Effect                                              |
|---------|---------|-----------------------------------------------------|
| `>`     | move    | Move the pointer one cell to the right              |
| `<`     | move    | Move the pointer one cell to the left               |
| `+`     | inc     | Increment the byte in the current cell (wrapping)   |
| `-`     | dec     | Decrement the byte in the current cell (wrapping)   |
| `.`     | out     | Write the current cell as a byte to stdout          |
| `,`     | in      | Read one byte from stdin into the current cell      |
| `[`     | loop-on | Jump forward past the matching `]` if the cell is 0 |
| `]`     | loop-off| Jump back to the matching `[` if the cell is not 0  |

## Examples

### 1. Hello World

File `hello.bf`:

```brainfuck
++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.
```

Run it:

```sh
$ ./target/release/interpreter hello.bf
Hello World!
```

### 2. A simple loop

This program multiplies 5 × 13 and prints the result (`5 * 13 = 65`, which is `'A'`):

```brainfuck
+++++
[
  >+++++++++++++
  <-
]
>.
```

Step by step:

1. `+++++` sets cell 0 to 5.
2. The loop runs while cell 0 is non-zero: it adds 13 to cell 1, then decrements cell 0.
3. After 5 iterations, cell 1 holds `5 * 13 = 65`.
4. `>.` moves to cell 1 and prints it as `'A'`.

```sh
$ ./target/release/interpreter mult.bf
A
```

### 3. Echo (cat program)

A program that copies stdin to stdout until it reads a `0` byte:

```brainfuck
,[.,]
```

```sh
$ printf 'hello' | ./target/release/interpreter cat.bf
hello
```

## Features

- **AST-based execution** — loops are compiled into nested `Loop` nodes once at parse time, so the interpreter never re-scans the source text. Executing a program is a straight tree walk.
- **Instruction folding** — runs of consecutive `+`/`-` are merged into a single `AddValue(n)` node, and runs of `>`/`<` into a single `MovePtr(n)` node. This makes tight numeric loops much faster than interpreting one character at a time.
- **Wrapping arithmetic** — cell values wrap modulo 256 (`u8`), so `255 + 1 == 0`.
- **Bracket balance checking** — unmatched `[` or `]` are caught at parse time and reported as errors instead of crashing.
- **Whitespace & comments tolerated** — all non-command characters are silently skipped.

## Memory Model

- Tape size: **30,000 cells**, fixed in `main()`.
- Cell type: `u8`, values wrap at 256.
- The pointer starts at cell 0 and moves via `MovePtr`.
- Moving the pointer out of bounds (negative or ≥ tape size) panics with a message.

## Error Handling

| Situation            | Behaviour                                          |
|----------------------|----------------------------------------------------|
| File can't be read   | `Error reading <file>: <reason>` on stderr         |
| Unmatched `]`        | `Error parsing <file>: Unmatched ']' found`        |
| Unmatched `[`        | `Error parsing <file>: Unmatched '[' found`        |
| Pointer out of range | Panic: `Pointer out of bounds: <idx>`              |
| Extra CLI arguments  | Warning printed, ignored                           |

## Implementation Details

Everything lives in `src/main.rs`:

- `parser(contents) -> Result<Vec<Node>, String>` — tokenizes the source into a `Vec<Node>`. A `stack` of `Vec<Node>` tracks open `[` blocks; `flush()` collapses consecutive additive/movement runs into single nodes. Returns an error on unbalanced brackets.
- `Executor { memory: Vec<u8>, ptr: usize }` — owns the tape and pointer. `execute()` recursively walks the AST, running `Loop` bodies in a `while` loop until the current cell becomes 0.
- `main()` — creates an executor with 30,000 cells and runs every CLI argument through `read_file` → `parser` → `execute`.

The folding behaviour can be seen directly: for `+++>---<`, the parser produces

```
AddValue(3), MovePtr(1), AddValue(-3), MovePtr(-1)
```

instead of eight separate single-step nodes.
