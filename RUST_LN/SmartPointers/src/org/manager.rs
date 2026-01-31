use super::behaviors::Behaviors;

pub struct Manager {
    name: String,
    age: i32,
}

impl Manager {
    pub fn new(_name: String, _age: i32) -> Self {
        Self {
            name: _name,
            age: _age,
        }
    }
}

impl Behaviors for Manager {
    fn say(&self) {
        println!("gailunfafa");
    }

    fn intro(&self) {
        println!("name: {}, age: {}, role: manager", self.name, self.age);
    }
}
