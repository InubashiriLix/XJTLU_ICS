use super::behaviors::Behaviors;

#[derive(Debug)]
pub enum Year {
    First,
    Second,
    Third,
    Fourth,
    Grad,
    Terminated,
}

pub struct Student {
    name: String,
    year: Year,
    age: i32,
}

impl Student {
    pub fn new(_name: String, _year: Year, age: i32) -> Self {
        Self {
            name: _name,
            year: _year,
            age,
        }
    }
}

impl Behaviors for Student {
    fn say(&self) {
        println!("gailun, fafa!");
    }
    fn intro(&self) {
        println!("name: {}, age: {}, role: student", self.name, self.age);
        println!("{:?}", self.year);
    }
}
