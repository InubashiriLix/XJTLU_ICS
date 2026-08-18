mod agent;
mod led;
use agent::{Event, State, run};

fn main() {
    let (state, commands) = run(
        State::initial_state(),
        [
            Event::Connected,
            Event::TaskStart { id: 1 },
            Event::TokenUsed { count: 10 },
            Event::TaskFinish { success: true },
            Event::TaskStart { id: 2 },
            Event::TokenUsed { count: 5 },
            Event::TaskFinish { success: false },
            Event::Disconnected,
        ],
    );
    println!("state: {:?}", state);
    println!("commands:");
    for command in commands {
        println!("  {:?}", command);
    }
}

