fn add(a: &i32, b: &i32) -> i32 {
    *a + *b
}

fn main() {
    println!("Hello world!");

    println!("add 2 and 3: {}", add(&2, &3));
    let sum = |a: i32, b: i32| -> i32 { a + b };
    println!("it is almost same with: {}", sum(2, 3));

    let vec: Vec<i32> = vec![1, 2, 3, 4, 5];
    // calcualte the sum of all the elements bigger than 3
    let sum: i32 = vec.iter().filter(|x| **x > 3).sum();
    // let sum: i32 = vec.iter().filter(|&&x| x > 3).sum();
    println!("{}", sum);

    // the closure will capture the env variables
    let shit_str: &str = "shit!";
    // let get_shit_fn = |input_str: &str| -> String { String::from(input_str) + " " + shit_str };
    let get_shit_fn = |input_str: &str| format!("{} {}", input_str, shit_str);
    println!("print somehting: {}", get_shit_fn("damn!"));

    // exe A:
    let add = |a: i32, b: i32| -> i32 { a + b };
    println!("add 1 and 2: {}", add(1, 2));

    // exe B
    let k = 91;
    let bigger = |input_int: &i32| -> bool { *input_int > k };
    let input_int: i32 = 100;
    println!("{} > 91? {}", input_int, bigger(&input_int));

    // exe C:
    let mut sum: i32 = 1;
    let mut sum_add = |input_int: &i32| {
        sum += *input_int;
    };
    let input_int: i32 = 32;
    sum_add(&input_int);
    println!("current sum: {}", sum);

    // .map_err(|e| format!("parse failed: {}", e))
    // this code will capture the error message and return a customized String type error message
}
