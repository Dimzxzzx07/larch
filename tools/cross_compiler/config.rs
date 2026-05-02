use std::path::PathBuf;

#[derive(Debug)]
pub struct CrossConfig {
    target: String,
    sysroot: PathBuf,
    c_compiler: String,
    cxx_compiler: String,
    rust_target: String,
}

impl CrossConfig {
    pub fn new() -> Self {
        CrossConfig {
            target: "x86_64-larch".to_string(),
            sysroot: PathBuf::from("/usr/local/larch-sysroot"),
            c_compiler: "x86_64-larch-gcc".to_string(),
            cxx_compiler: "x86_64-larch-g++".to_string(),
            rust_target: "x86_64-unknown-larch".to_string(),
        }
    }
    
    pub fn generate_rust_target_json(&self) -> String {
        format!(r#"{{
            "llvm-target": "x86_64-unknown-none",
            "data-layout": "e-m:e-i64:64-f80:128-n8:16:32:64-S128",
            "arch": "x86_64",
            "target-endian": "little",
            "target-pointer-width": "64",
            "target-c-int-width": "32",
            "os": "none",
            "executables": true,
            "linker-flavor": "ld.lld",
            "panic-strategy": "abort",
            "disable-redzone": true,
            "features": "-mmx,-sse,+soft-float"
        }}"#)
    }
    
    pub fn generate_ld_script(&self) -> String {
        String::from(r#"
ENTRY(_start)

SECTIONS {
    . = 0x100000;
    
    .text : {
        *(.text*)
    }
    
    .rodata : {
        *(.rodata*)
    }
    
    .data : {
        *(.data*)
    }
    
    .bss : {
        *(.bss*)
        *(COMMON)
    }
}
        "#)
    }
}

fn main() {
    let config = CrossConfig::new();
    println!("Target: {}", config.target);
    println!("Sysroot: {}", config.sysroot.display());
    println!("C compiler: {}", config.c_compiler);
    println!("Rust target JSON:\n{}", config.generate_rust_target_json());
}