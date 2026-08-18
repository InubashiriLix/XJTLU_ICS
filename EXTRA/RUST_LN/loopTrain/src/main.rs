fn is_even(num: i32) -> bool {
    num % 2 == 0
}

fn sum_from_to(start: i32, end: i32) -> i32 {
    let mut sum = 0;
    for i in start..end {
        sum += i;
    }
    sum
}

fn main() {
    println!("number 4 is even: {}", is_even(4));
    println!("number 5 is even: {}", is_even(5));

    println!("the number from 1 to 10 that are even:");
    for i in 1..11 {
        if is_even(i) {
            println!("{}", i);
        }
    }

    println!("the number from 1 to 10 that are odd:");
    for i in 1..11 {
        if !is_even(i) {
            println!("{}", i);
        }
    }

    println!("the sum from 1 to 10: {}", sum_from_to(1, 11));

    let start: i32 = 1;
    let end: i32 = 10;
    let mut sum: i32 = 0;
    let mut curr_num: i32 = start;
    while curr_num <= end {
        sum += curr_num;
        curr_num += 1;
    }
    println!("the sum from {} to {} is {}", start, end, sum);
}
