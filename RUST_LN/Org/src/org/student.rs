use super::member::Member;

#[derive(Debug)]
pub struct Student<'a> {
    name: &'a str,
    role: &'a str,
    age: u8,
    work: &'a str,
}

impl<'a> Student<'a> {
    pub fn new(name: &'a str, age: u8) -> Result<Self, String> {
        if name.is_empty() {
            return Err("The name can not be empty".to_string());
        }
        if age > 150 {
            return Err("Invalid Age".to_string());
        }

        return Ok(Student {
            name: name,
            role: "student",
            age: age,
            work: "no job",
        });
    }
}

impl<'a> Member for Student<'a> {
    fn name(&self) -> &str {
        self.name
    }

    fn role(&self) -> &str {
        self.role
    }

    fn age(&self) -> u8 {
        self.age
    }

    fn work(&self) -> &str {
        self.work
    }
}
