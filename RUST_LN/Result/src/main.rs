fn parse_i32(s: &str) -> Result<i32, String> {
    match s.parse::<i32>() {
        Ok(v) => Ok(v),
        Err(_) => Err(format!("Failed to parse '{}' as i32", s)),
    }
}

fn parse_i32_v2(s: &str) -> Result<i32, String> {
    let v = s.parse::<i32>().map_err(|_| format!("connot parse {}", s))?;
    Ok(v)
}

fn main() {
    println!("Hello, world!");

    match parse_i32("91") {
        Ok(v) => println!("Parsed value: {}", v),
        Err(e) => println!("Error: {}", e),
    }

    match parse_i32("this is a shit") {
        Ok(v) => println!("Parsed value: {}", v),
        Err(e) => println!("Error: {}", e),
    }
}
