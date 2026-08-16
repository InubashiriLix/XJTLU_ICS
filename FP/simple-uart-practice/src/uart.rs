#[derive(Debug)]
pub enum Command {
    Led(bool),
    Pwm(u8),
    Motor(u16),
    Reset,
}

#[derive(Debug)]
pub enum ParseError {
    Empty,
    UnknownCommand,
    MissingArgument,
    InvalidNumber,
    OutOfRange,
}

pub fn parse_command(input: &str) -> Result<Command, ParseError> {
    let mut parts = input.split(" ").map(|s| s.trim());

    let cmd = match parts.next() {
        Some(name) if !name.is_empty() => name as &str,
        _ => return Err(ParseError::Empty),
    };

    match cmd {
        "reset" => Ok(Command::Reset),
        "led" => {
            let arg = parts.next().ok_or(ParseError::MissingArgument)?;
            match arg {
                "on" => Ok(Command::Led(true)),
                "off" => Ok(Command::Led(false)),
                _ => Err(ParseError::UnknownCommand),
            }
        }
        "pwm" | "motor" => {
            let arg = parts.next().ok_or(ParseError::MissingArgument)?;
            let v: u32 = arg.parse().map_err(|_| ParseError::InvalidNumber)?;
            if cmd == "pwm" {
                if v > u8::MAX as u32 {
                    Err(ParseError::OutOfRange)
                } else {
                    Ok(Command::Pwm(v as u8))
                }
            } else {
                if v > u16::MAX as u32 {
                    Err(ParseError::OutOfRange)
                } else {
                    Ok(Command::Motor(v as u16))
                }
            }
        }
        _ => Err(ParseError::UnknownCommand),
    }
}

impl std::fmt::Display for ParseError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ParseError::Empty => write!(f, "Empty"),
            ParseError::UnknownCommand => write!(f, "UnknownCommand"),
            ParseError::MissingArgument => write!(f, "MissingArgument"),
            ParseError::InvalidNumber => write!(f, "InvalidNumber"),
            ParseError::OutOfRange => write!(f, "OutOfRange"),
        }
    }
}
