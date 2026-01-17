fn parse_i32(input_str: &str) {
    match input_str.trim().parse::<i32>() {
        Err(e) => println!("cannot parse {}: {}", input_str, e),
        Ok(v) => println!("parsed ok: {}", v),
    }
}

fn parse_i32_question(input_str: &str) -> Result<i32, String> {
    let ret = input_str
        .trim()
        .parse::<i32>()
        .map_err(|e| format!("cannot parse {}: {}", input_str, e))?;
    Ok(ret)
}

struct User {
    name: String,
    age: u8,
}

impl User {
    fn new(name: &str, age: u8) -> Result<Self, String> {
        if name.is_empty() {
            return Err("name cannot be empty".to_string());
        }
        if age == 0 || age > 150 {
            return Err("age must be between 1 and 150".to_string());
        }
        Ok(User { name: name.to_string(), age })
    }

    fn say_hello(&self) {
        println!("hello, my name is {}, age {}", self.name, self.age);
    }

    fn get_name(&self) -> &str {
        return self.name.as_str();
    }
}

#[derive(Debug)]
struct Point {
    x: f64,
    y: f64,
}

fn parse_point(input_str: &str) -> Result<Point, String> {
    // first, the desireable input string are like "1.0,2.0"
    // we first split by ','
    let split_vec = input_str.trim().split(',').collect::<Vec<&str>>();
    if split_vec.len() != 2 {
        return Err(format!(
            "the input string {} is not valid point format",
            input_str
        ));
    } else {
        // x part:
        let x_part = split_vec[0].parse::<f64>().map_err(|e| {
            format!("cannot parse x part {} as f64: {}", split_vec[0], e)
        })?;
        let y_part = split_vec[1].parse::<f64>().map_err(|e| {
            format!("cannot parse y part {} as f64: {}", split_vec[1], e)
        })?;
        return Ok(Point { x: x_part, y: y_part });
    }
}

fn distance_from_str(a: &str, b: &str) -> Result<f64, String> {
    let p1 = parse_point(a)?;
    let p2 = parse_point(b)?;
    return Ok(((p1.x - p2.x).powi(2) + (p1.y - p2.y).powi(2)).sqrt());
}

fn main() {
    println!("-- parse_i32 --");
    parse_i32("1234");

    println!("try and catch");
    match parse_i32_question("1234") {
        Err(e) => println!("{}", e),
        Ok(v) => println!("parsed ok: {}", v),
    }

    match parse_i32_question("1234!") {
        Err(e) => println!("{}", e),
        Ok(v) => println!("parsed ok: {}", v),
    }

    // any faster way to write above code? help me gpt
    // method 1: using if let, but its short cut is: double work for both ok and err: for the
    // functions that have side effect, its not a good idea.
    // if let Ok(v) = parse_i32_question("5678") {
    //     println!("parsed ok: {}", v);
    // } else if let Err(e) = parse_i32_question("5678!") {
    //     println!("{}", e);
    // }
    // method 2: using unwrap_or_else
    // let v = parse_i32_question("5678").unwrap_or_else(|e| {
    //     println!("{}", e);
    //     -1 // the default value when error occurs
    // });
    // method3: map_or_else
    // let v2 = parse_i32_question("5678!")
    //     .map_or_else(|e| println!("{}", e), |v| println!("parsed ok: {}", v));

    // find user example:
    let user1: User = User::new("Alice", 30).unwrap();
    let user2: User = User::new("Bob", 15).unwrap();
    let user3: User = User::new("Cathy", 15).unwrap();
    let vec_users: Vec<&User> = vec![&user1, &user2, &user3];

    // i know this could be shit cause this will cause the users and user_vec could not be motified
    // untill find_user_and_say_hello's final ref. but fuck it.
    let find_user_and_say_hello = |name: &str| match vec_users
        .iter()
        .find(|user| user.get_name() == name)
    {
        Some(user) => user.say_hello(),
        None => println!("user called {} does not exist.", name),
    };

    find_user_and_say_hello("shit");
    find_user_and_say_hello("Alice");

    // well, lets start execrise
    // Exe A
    match parse_point("1.5,1.5") {
        Ok(v) => println!("{:?}", v),
        Err(_) => (),
    }

    // Exe B
    match distance_from_str("1.0,1.0", "2.0,2.0") {
        Ok(v) => println!("{:?}", v),
        Err(_) => (),
    }

    // Exe 3
    // > **为什么 `Result` 比异常更适合系统级语言？**
    // rust has defined each exception must be handled using `Result` (safer) and It can contain more
    // specific exception type or msg. In comparision with c, we usually use  state code to
    // represents error, which could be fuzzy and hard to memorize.
}
