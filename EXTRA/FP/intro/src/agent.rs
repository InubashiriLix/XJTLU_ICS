#[derive(Debug)]
pub enum Event {
    Connected,
    Disconnected,
    TaskStart { id: u32 },
    TokenUsed { count: u32 },
    TaskFinish { success: bool },
}

#[derive(Debug)]
pub enum Command {
    SetLedIdle,
    SetLedWorking,
    SetLedSuccess,
    SetLedError,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub enum Connection {
    Offline,
    Online,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct State {
    connection: Connection,
    active_task: Option<u32>,
    total_token: u32,
    completed_task: u32,
    failed_task: u32,
}

impl State {
    pub fn initial_state() -> State {
        State {
            connection: Connection::Offline,
            active_task: None,
            total_token: 0,
            completed_task: 0,
            failed_task: 0,
        }
    }

    pub fn reduce(state: State, event: Event) -> (State, Vec<Command>) {
        match event {
            Event::Connected => (
                State {
                    connection: Connection::Online,
                    ..state
                },
                vec![Command::SetLedWorking],
            ),
            Event::Disconnected => (
                State {
                    connection: Connection::Offline,
                    active_task: None,
                    ..state
                },
                vec![],
            ),
            Event::TaskStart { id } => {
                if state.connection == Connection::Online {
                    (
                        State {
                            active_task: Some(id),
                            ..state
                        },
                        vec![Command::SetLedWorking],
                    )
                } else {
                    (state, vec![])
                }
            }
            Event::TokenUsed { count } => {
                if state.active_task.is_some() {
                    (
                        State {
                            total_token: state.total_token + count,
                            ..state
                        },
                        vec![Command::SetLedIdle],
                    )
                } else {
                    (state, vec![])
                }
            }
            Event::TaskFinish { success } => {
                if state.active_task.is_some() {
                    if success {
                        (
                            State {
                                completed_task: state.completed_task + 1,
                                active_task: None,
                                ..state
                            },
                            vec![Command::SetLedSuccess],
                        )
                    } else {
                        (
                            State {
                                failed_task: state.failed_task + 1,
                                active_task: None,
                                ..state
                            },
                            vec![Command::SetLedError],
                        )
                    }
                } else {
                    (state, vec![])
                }
            }
        }
    }
}

pub fn run(initial: State, events: impl IntoIterator<Item = Event>) -> (State, Vec<Command>) {
    events
        .into_iter()
        .fold((initial, Vec::new()), |(state, mut cmds), events| {
            let (new_state, new_cmds) = State::reduce(state, events);
            cmds.extend(new_cmds);
            (new_state, cmds)
        })
}
