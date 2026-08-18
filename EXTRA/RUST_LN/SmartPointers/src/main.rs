mod org;

use org::behaviors::Behaviors;
use org::manager::Manager;
use org::student::Student;
use org::student::Year;
use org::team::Team;

use std::cell::RefCell;
use std::rc::Rc;

enum List {
    Cons(i32, Box<List>),
    None,
}

fn main() {
    let shit = Box::new(5);
    println!("shit: {}", shit);

    let mut team: Team = Team::new();

    let manager = Manager::new("Fafa".to_string(), 35);
    let student = Student::new("Gailun".to_string(), Year::First, 18);
    team.add(Box::new(manager));
    team.add(Box::new(student));

    team.say_something();
    team.intro_everyone();

    let l = List::Cons(
        1,
        Box::new(List::Cons(2, Box::new(List::Cons(3, Box::new(List::None))))),
    );

    let s = Rc::new(String::from("this is a shit"));
    let a = Rc::clone(&s);
    let b = Rc::clone(&s);
    println!("strong count: {}", Rc::strong_count(&s));
    println!("{}", b);

    let x = RefCell::new(5);
    *x.borrow_mut() += 1;
    println!("{}", x.borrow());

    // let _r = x.borrow();
    // let _w = x.borrow_mut();
}
