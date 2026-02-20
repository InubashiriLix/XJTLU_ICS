use std::fs::File;
use std::io::prelude::*;

#[derive(Debug)]
enum Node {
    MovePtr(i32),
    AddValue(i32),
    Input,
    Output,
    Loop(Vec<Node>),
}

#[derive(Debug, PartialEq)]
enum State {
    Idle,
    AccumeValue,
    AccumePtr,
}

fn read_file(file_path: &str) -> Result<String, String> {
    let mut file = File::open(file_path).map_err(|e| format!("cannot read: {}", e))?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)
        .map_err(|e| format!("cannot read: {}", e))?;
    Ok(contents)
}

fn flush(state: &mut State, temp_ops_count: &mut i32, cur_vec: &mut Vec<Node>) {
    match state {
        State::AccumeValue => {
            if *temp_ops_count != 0 {
                cur_vec.push(Node::AddValue(*temp_ops_count));
            }
        }
        State::AccumePtr => {
            if *temp_ops_count != 0 {
                cur_vec.push(Node::MovePtr(*temp_ops_count));
            }
        }
        State::Idle => {}
    }

    *temp_ops_count = 0;
    *state = State::Idle;
}

fn parser(contents: &String) -> Result<Vec<Node>, String> {
    let mut state = State::Idle;
    let mut stack: Vec<Vec<Node>> = Vec::new();
    let mut current_vec: Vec<Node> = Vec::new();
    let mut temp_ops_count: i32 = 0;

    for ch in contents.chars() {
        match ch {
            '+' => {
                if state != State::AccumeValue {
                    flush(&mut state, &mut temp_ops_count, &mut current_vec);
                    state = State::AccumeValue;
                }
                temp_ops_count += 1;
            }
            '-' => {
                if state != State::AccumeValue {
                    flush(&mut state, &mut temp_ops_count, &mut current_vec);
                    state = State::AccumeValue;
                }
                temp_ops_count -= 1;
            }

            '>' => {
                if state != State::AccumePtr {
                    flush(&mut state, &mut temp_ops_count, &mut current_vec);
                    state = State::AccumePtr;
                }
                temp_ops_count += 1;
            }

            '<' => {
                if state != State::AccumePtr {
                    flush(&mut state, &mut temp_ops_count, &mut current_vec);
                    state = State::AccumePtr;
                }
                temp_ops_count -= 1;
            }

            '.' => {
                flush(&mut state, &mut temp_ops_count, &mut current_vec);
                current_vec.push(Node::Output);
            }
            ',' => {
                flush(&mut state, &mut temp_ops_count, &mut current_vec);
                current_vec.push(Node::Input);
            }

            '[' => {
                flush(&mut state, &mut temp_ops_count, &mut current_vec);
                // push current context to stack
                stack.push(current_vec);
                // start new loop body
                current_vec = Vec::new();
            }
            ']' => {
                flush(&mut state, &mut temp_ops_count, &mut current_vec);

                let loop_body = current_vec;
                // restore previous context
                current_vec = stack.pop().ok_or("Unmatched ']' found".to_string())?;
                current_vec.push(Node::Loop(loop_body));
            }

            _ => {
                // ignore non-brainfuck characters like comments or whitespace
            }
        }
    }

    flush(&mut state, &mut temp_ops_count, &mut current_vec);

    if !stack.is_empty() {
        return Err("Unmatched '[' found".to_string());
    }

    Ok(current_vec)
}

struct Executor {
    memory: Vec<u8>,
    ptr: usize,
}

impl Executor {
    fn new(mem_size: u32) -> Result<Self, String> {
        if mem_size == 0 {
            return Result::Err("Memory size must be a positive integer".to_string());
        }

        Ok(Self {
            memory: vec![0; mem_size as usize],
            ptr: 0,
        })
    }

    fn execute(&mut self, nodes: &Vec<Node>) {
        for node in nodes {
            match node {
                Node::AddValue(val) => {
                    let cell = &mut self.memory[self.ptr];
                    *cell = cell.wrapping_add(*val as u8);
                }

                Node::MovePtr(offset) => {
                    let new_ptr = self.ptr as i32 + offset;
                    if new_ptr < 0 || new_ptr >= self.memory.len() as i32 {
                        panic!("Pointer out of bounds: {}", new_ptr);
                    }
                    self.ptr = new_ptr as usize;
                }

                Node::Output => {
                    print!("{}", self.memory[self.ptr] as char);
                    std::io::stdout().flush().unwrap();
                }

                Node::Input => {
                    let mut buffer = [0u8];
                    std::io::stdin().read_exact(&mut buffer).unwrap();
                    self.memory[self.ptr] = buffer[0];
                }

                Node::Loop(body) => {
                    while self.memory[self.ptr] != 0 {
                        self.execute(body);
                    }
                }
            }
        }
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    let mut executor = Executor::new(30000)
        .map_err(|e| format!("Failed to initialize executor: {}", e))
        .unwrap();

    for (i, arg) in args.iter().enumerate() {
        if i > 1 {
            println!("ignoring extra argument: {}", arg);
        }

        match read_file(arg) {
            Ok(contents) => match parser(&contents) {
                Ok(nodes) => {
                    executor.execute(&nodes);
                }
                Err(e) => eprintln!("Error parsing file {}: {}", arg, e),
            },
            Err(e) => eprintln!("Error reading file {}: {}", arg, e),
        }
    }
}
