#![no_std]

use core::ptr::null_mut;

#[derive(Clone, Copy)]
pub enum ProcessState {
    Ready,
    Running,
    Waiting,
    Terminated,
}

pub struct Process {
    pub pid: u32,
    pub state: ProcessState,
    pub stack_ptr: *mut u32,
    pub page_directory: *mut u32,
    pub next: Option<&'static mut Process>,
}

impl Process {
    pub fn new(pid: u32) -> Self {
        Process {
            pid,
            state: ProcessState::Ready,
            stack_ptr: null_mut(),
            page_directory: null_mut(),
            next: None,
        }
    }
}

pub struct Scheduler {
    processes: [Option<Process>; 256],
    current: usize,
    count: usize,
}

impl Scheduler {
    pub const fn new() -> Self {
        Scheduler {
            processes: [None; 256],
            current: 0,
            count: 0,
        }
    }
    
    pub fn add_process(&mut self, process: Process) {
        if self.count < 256 {
            self.processes[self.count] = Some(process);
            self.count += 1;
        }
    }
    
    pub fn next(&mut self) -> Option<&mut Process> {
        if self.count == 0 {
            return None;
        }
        
        let start = self.current;
        loop {
            self.current = (self.current + 1) % self.count;
            if let Some(ref mut proc) = self.processes[self.current] {
                match proc.state {
                    ProcessState::Ready => return Some(proc),
                    ProcessState::Running => return Some(proc),
                    _ => {
                        if self.current == start {
                            return None;
                        }
                    }
                }
            }
        }
    }
    
    pub fn current_mut(&mut self) -> Option<&mut Process> {
        self.processes[self.current].as_mut()
    }
    
    pub fn current_pid(&self) -> Option<u32> {
        self.processes[self.current].as_ref().map(|p| p.pid)
    }
}