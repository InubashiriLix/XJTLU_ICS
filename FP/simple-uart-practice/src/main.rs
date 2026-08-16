mod uart;
use uart::{Command, ParseError};

fn main() {
    match uart::parse_command("pwm fuck") {
        Ok(cmd) => println!("ok: {cmd:?}"),
        Err(e) => println!("err: {e}"),
    }

    println!("Hello, world!");
}
