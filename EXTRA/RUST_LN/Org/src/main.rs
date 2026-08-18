mod org;

use org::manager::Manager;
use org::student::Student;
use org::team::Team;

fn main() {
    let mut team = Team::new();
    let mut student_alice = Student::new("Alice", 20).unwrap();
    let mut manager_bob = Manager::new("Bob", "Managing Team", 35, 5).unwrap();

    team.add(Box::new(student_alice));
    team.add(Box::new(manager_bob));

    team.daily_work();
}
