use super::member::Member;

#[derive(Debug)]
pub struct Manager<'a> {
    _name: &'a str,
    _team_size: u32,
    _age: u8,
}

impl<'a> Manager<'a> {
    pub fn new(name: &'a str, work: &str, age: u8, team_size: u32) -> Result<Self, String> {
        if name.is_empty() {
            return Err("name should not be empty".to_string());
        }
        if age > 150 {
            return Err("Invalid Age".to_string());
        }
        if work.is_empty() {
            return Err("work should not be empty".to_string());
        }
        return Ok(Manager {
            _name: name,
            _team_size: team_size,
            _age: age,
        });
    }

    pub fn team_size(&self) -> u32 {
        self._team_size
    }
}

impl<'a> Member for Manager<'a> {
    fn name(&self) -> &str {
        self._name
    }

    fn role(&self) -> &str {
        "manager"
    }

    fn work(&self) -> &str {
        "manager"
    }

    fn age(&self) -> u8 {
        self._age
    }
}
