use super::member::Member;

pub struct Team {
    members: Vec<Box<dyn Member>>,
}

impl Team {
    pub fn new() -> Self {
        Team {
            members: Vec::new(),
        }
    }

    pub fn add(&mut self, m: Box<dyn Member>) {
        self.members.push(m);
    }

    pub fn daily_work(&self) {
        for m in &self.members {
            println!("{} ({})", m.name(), m.role());
            m.work();
        }
    }
}
