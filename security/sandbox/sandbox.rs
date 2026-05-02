#![no_std]

#[derive(Clone, Copy)]
pub struct SandboxPolicy {
    pub allow_filesystem: bool,
    pub allow_network: bool,
    pub allow_audio: bool,
    pub max_memory_mb: u32,
    pub allowed_paths: [u32; 8],
}

impl SandboxPolicy {
    pub const RESTRICTED: Self = SandboxPolicy {
        allow_filesystem: false,
        allow_network: false,
        allow_audio: false,
        max_memory_mb: 64,
        allowed_paths: [0; 8],
    };
    
    pub const NETWORK_ONLY: Self = SandboxPolicy {
        allow_filesystem: false,
        allow_network: true,
        allow_audio: false,
        max_memory_mb: 128,
        allowed_paths: [0; 8],
    };
    
    pub const FULL: Self = SandboxPolicy {
        allow_filesystem: true,
        allow_network: true,
        allow_audio: true,
        max_memory_mb: 1024,
        allowed_paths: [0; 8],
    };
    
    pub fn add_allowed_path(&mut self, path_hash: u32, index: usize) {
        if index < 8 {
            self.allowed_paths[index] = path_hash;
        }
    }
    
    pub fn can_access_path(&self, path_hash: u32) -> bool {
        if !self.allow_filesystem {
            return false;
        }
        for i in 0..8 {
            if self.allowed_paths[i] == path_hash {
                return true;
            }
        }
        false
    }
}

pub struct Sandbox {
    policy: SandboxPolicy,
    memory_used: u32,
}

impl Sandbox {
    pub const fn new(policy: SandboxPolicy) -> Self {
        Sandbox {
            policy,
            memory_used: 0,
        }
    }
    
    pub fn check_alloc(&mut self, size_mb: u32) -> bool {
        if self.memory_used + size_mb > self.policy.max_memory_mb {
            false
        } else {
            self.memory_used += size_mb;
            true
        }
    }
    
    pub fn free_memory(&mut self, size_mb: u32) {
        if size_mb <= self.memory_used {
            self.memory_used -= size_mb;
        } else {
            self.memory_used = 0;
        }
    }
    
    pub fn policy(&self) -> &SandboxPolicy {
        &self.policy
    }
}