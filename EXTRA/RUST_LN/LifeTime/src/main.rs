// return a local raiable's ref is forbidden
// fn foo() -> &str {
//     let s: String = String::from("hello");
//     &s
// }

// require the lifetime annotation
// fn longer(x: &str, y: &str) -> &str {
//     if x.len() > y.len() { x } else { y }
// }

// the 'a here is the intersection of x and y's lifetime
// this express that the resutned reference will live as long as both x and y
fn longer<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() > y.len() { x } else { y }
}

struct User<'a> {
    name: &'a str,
}

// impl<'a> User<'a> {
//     fn name(&self) -> &str {
//         self.name
//     }
// }

// no escape elision
impl<'a> User<'a> {
    fn name<'b>(&'b self) -> &'b str {
        self.name
    }
}

// exe1
// the returned value should live as long as the lifetime of intersection of x and y
fn first<'a>(x: &'a str, y: &'a str) -> &'a str {
    x
}

// exe2
// holder should not live longer than the value it holds
struct Holder<'a> {
    value: &'a i32,
}

// exe3
// we do not require to explicitly annotate the lifetime here because of elision rules
struct Student<'a> {
    name: &'a str,
    age: u32,
}

trait Member {
    fn name(&self) -> &str;
}

impl<'a> Member for Student<'a> {
    fn name(&self) -> &'a str {
        self.name
    }
}

fn main() {
    println!("Hello, world!");
    println!("{}", longer("hello", "hi"));

    let user = User { name: "Alice" };
    println!("User name: {}", user.name());

    println!("======= exercise 1 =======");

    println!("======= exercise 2 =======");
    println!("======= exercise 3 =======");
}
