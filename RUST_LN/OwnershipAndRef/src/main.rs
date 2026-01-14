fn string_len(s: &String) -> usize {
    s.len()
}

fn append_world(s: &mut String) {
    s.push_str(" world");
}

fn main() {
    let mut s = String::from("hello");
    println!("{}", s);
    println!("len of{}: {}", s, string_len(&s));
    append_world(&mut s);
    println!("{}", s);

    let mut s = String::from("hello");
    {
        let r1 = &s;
        println!("r1: {}", r1);
    }
    let r2 = &mut s;

    println!("{}", r2);
}
