use super::behaviors::Behaviors;

pub struct Team {
    members: Vec<Box<dyn Behaviors>>,
}

impl Team {
    pub fn new() -> Self {
        Self {
            members: Vec::new(),
        }
    }

    pub fn size(&self) -> usize {
        return self.members.len();
    }

    pub fn add(&mut self, m: Box<dyn Behaviors>) {
        self.members.push(m);
    }

    pub fn say_something(&self) {
        self.members.iter().for_each(|mem| {
            mem.say();
        })
    }

    pub fn intro_everyone(&self) {
        self.members.iter().for_each(|m| {
            m.intro();
        })
    }
}
