# Larch OS

BSD 3-Clause License | Platform: i686

A minimalist, educational operating system built from scratch with C, C++, and Rust. Larch OS is designed to be flexible, secure, and learnable.

## Table of Contents

- [What is Larch OS?](#what-is-larch-os)
- [Features](#features)
- [Known Limitations](#known-limitations)
- [Architecture](#architecture)
- [System Requirements](#system-requirements)
- [Installation](#installation)
  - [From ISO (QEMU)](#from-iso-qemu)
  - [From ISO (Bare Metal)](#from-iso-bare-metal)
  - [Build from Source](#build-from-source)
- [Quick Start](#quick-start)
- [Shell Commands](#shell-commands)
- [Directory Structure](#directory-structure)
- [Building the OS](#building-the-os)
- [Development](#development)
- [Troubleshooting](#troubleshooting)
- [FAQ](#faq)
- [Contributing](#contributing)
- [Terms of Service](#terms-of-service)
- [License](#license)

## What is Larch OS?

Larch OS is a hobby operating system written in C, C++, and Rust. It boots via GRUB with Multiboot2, runs in 32-bit protected mode, and features a custom kernel with memory management, process scheduling, filesystem support, and an interactive shell.

It is not a production OS. It is a learning platform and a foundation for building your own operating system.

## Features

| Feature | Status | Description |
|---------|--------|-------------|
| Bootloader (GRUB) | Yes | Multiboot2 compliant |
| GDT/IDT | Yes | Protected mode setup |
| PMM (Physical Memory Manager) | Yes | Bitmap-based allocation |
| VMM (Virtual Memory Manager) | Yes | Paging support |
| Process Scheduler | Yes | Round-robin multitasking |
| PIT Timer | Yes | 100Hz interrupts |
| Keyboard Driver | Yes | PS/2 scancode translation |
| Mouse Driver | Yes | PS/2 mouse support |
| Shell | Yes | Built-in commands |
| Initramfs | Yes | CPIO archive loading |
| Syscalls | Yes | int 0x80 interface |
| VGA Text Mode | Yes | 80x25 with scrolling |
| Command History | Yes | Up/down arrow navigation |

## Known Limitations

These are honest limitations of the current version:

| Limitation | Description |
|------------|-------------|
| Read-only filesystem | No persistent storage; initramfs is read-only |
| No POSIX compatibility | Custom syscall ABI, not Linux-compatible |
| Incomplete sys_fork | Process duplication not fully implemented |
| Single-user | No multi-user support yet |
| No networking stack | Network drivers present but no TCP/IP |
| 32-bit only | No x86_64 long mode support |
| VGA text only | No framebuffer GUI yet |
| No ACPI | Shutdown/reboot via port 0x64 only |

## Architecture

Layer Structure:

- User Programs (Ring 3) - Planned
- Shell Interface
- Syscall Interface (int 0x80)
- VFS + Scheduler + ELF Loader
- PMM + VMM + GDT/IDT + PIT + Drivers
- x86 Hardware

Language breakdown by layer:

- Kernel core (GDT/IDT/PMM/VMM): C and Rust
- Process scheduler and drivers: C and Rust
- Shell and builtins: C
- Future GUI compositor: C++

## System Requirements

Minimum:

- CPU: Intel/AMD x86 32-bit (i686) or 64-bit with 32-bit support
- RAM: 512 MB
- Storage: 100 MB free space
- Boot: BIOS or UEFI (with CSM) / GRUB-compatible bootloader

Recommended:

- RAM: 1 GB or more
- CPU: Any x86-64 processor
- Graphics: VGA-compatible or framebuffer support

Supported Emulators:

- QEMU (i386 or x86_64 with -cpu qemu32)
- Bochs
- VirtualBox (legacy BIOS mode)

## Installation

### From ISO (QEMU)

1. Download `larch.iso` from releases
2. Install QEMU:
   - Ubuntu/Debian: `sudo apt install qemu-system-x86`
   - Arch Linux: `sudo pacman -S qemu`
   - macOS: `brew install qemu`
3. Run the OS:
   ```bash
   qemu-system-i386 -cdrom larch.iso -m 512
```

From ISO (Bare Metal)

WARNING: This is experimental software. Back up your data first.

1. Write ISO to USB drive:
   ```bash
   sudo dd if=larch.iso of=/dev/sdX bs=4M status=progress
   ```
   (Replace /dev/sdX with your USB device)
2. Boot from USB in legacy BIOS mode
3. Use the shell interface

Build from Source

1. Install dependencies:
   ```bash
   # Ubuntu/Debian
   sudo apt install build-essential gcc-12-plugin-dev nasm grub-pc-bin xorriso qemu-system-x86
   
   # Install i686 cross-compiler (Ubuntu/Debian)
   sudo apt install gcc-i686-linux-gnu
   
   # Or build from source: https://wiki.osdev.org/GCC_Cross-Compiler
   
   # Arch Linux
   sudo pacman -S base-devel nasm grub xorriso qemu
   
   # Install Rust
   curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
   rustup target add i686-unknown-none
   ```
2. Clone repository:
   ```bash
   git clone https://github.com/Dimzxzzx07/larch.git
   cd larch
   ```
3. Build:
   ```bash
   make clean
   make larch.iso
   ```
4. Run in QEMU:
   ```bash
   make run
   ```

Quick Start

When Larch OS boots, you will see the shell prompt:

```
[larch] $>
```

Type help to see available commands.

Basic workflow:

1. ls - List files in initramfs
2. cat bin/hello - Display file contents
3. uptime - Check system uptime
4. mem - View memory information
5. uname - Display OS version

Shell Commands

Command Description
help Show all available commands
clear Clear the screen
reboot Reboot the system
ls List files in initramfs
cat <file> Display file contents
echo <text> Print text to screen
mem Show memory usage
uptime Show system uptime
uname Show OS information
hexdump <file> Hex dump of file
date Show current time from RTC
top Show running processes

Navigation:

· Up arrow - Previous command
· Down arrow - Next command

Directory Structure

```
larch/
├── kernel/
│   ├── larch/          # Core kernel (GDT, IDT, boot, multiboot)
│   ├── gdt/            # GDT with TSS entry
│   ├── mem/            # Memory management (PMM, VMM in Rust)
│   ├── sched/          # Process scheduler and context switch
│   ├── drivers/        # VGA, keyboard, mouse, PIT, serial, RTC
│   ├── syscall/        # System call handlers
│   ├── fs/             # Virtual filesystem and initramfs
│   ├── shell/          # Command-line interface with history
│   ├── printf/         # Kernel printing (kprintf)
│   └── userspace/      # ELF loader
├── usr/
│   ├── bin/            # User binaries
│   └── include/        # Headers
├── tools/              # Build tools
├── Makefile            # Build system
├── LICENSE             # BSD 3-Clause
├── TOS.md              # Terms of Service
└── README.md
```

Building the OS

Makefile Targets

Command Description
make or make larch.iso Build the ISO image
make run Build and run in QEMU
make run-debug Run with serial output + interrupt logging
make clean Remove build artifacts
make clean-all Remove everything including initramfs
make info Show build configuration and available targets

Manual Build Steps

1. Compile bootloader assembly:
   ```bash
   i686-elf-as -felf32 kernel/larch/boot.s -o build/boot.o
   ```
2. Compile C kernel files:
   ```bash
   i686-elf-gcc -std=c99 -ffreestanding -nostdlib -I./kernel -c kernel/gdt/gdt_full.c -o build/gdt_full.o
   ```
3. Compile Rust kernel files:
   ```bash
   rustc --target i686-unknown-none -C opt-level=3 -C lto --emit obj kernel/mem/pmm.rs -o build/pmm.o
   ```
4. Link kernel:
   ```bash
   i686-elf-gcc -T linker.ld -ffreestanding -nostdlib build/*.o -o larch.bin
   ```
5. Create initramfs:
   ```bash
   cd initramfs && find . | cpio -o -H newc > ../initramfs.cpio
   ```
6. Create ISO:
   ```bash
   grub-mkrescue -o larch.iso iso/
   ```

Development

Debugging with GDB

1. Start QEMU with debug server:
   ```bash
   qemu-system-i386 -cdrom larch.iso -s -S
   ```
2. In another terminal, connect GDB:
   ```bash
   i686-elf-gdb
   (gdb) target remote localhost:1234
   (gdb) symbol-file larch.bin
   (gdb) break kernel_main
   (gdb) continue
   ```

Using run-debug

The make run-debug target provides:

· Serial output via -serial stdio
· Interrupt logging via -d int,cpu_reset

This is useful for tracking page faults, exceptions, and CPU resets.

Adding a New Syscall

1. Add number in kernel/syscall/syscall_handler.c:
   ```c
   #define SYS_NEWCALL 14
   ```
2. Implement handler in switch statement:
   ```c
   case SYS_NEWCALL:
       return sys_newcall(arg1);
   ```
3. Add implementation in syscall_implementations.c:
   ```c
   uint32_t sys_newcall(uint32_t arg) {
       return arg * 2;
   }
   ```
4. User space wrapper in usr/include/larch.h:
   ```c
   static inline int newcall(int arg) {
       return syscall(SYS_NEWCALL, arg, 0, 0);
   }
   ```

Troubleshooting

Problem Solution
QEMU shows "not a bootable disk" Ensure GRUB is installed: sudo apt install grub-pc-bin
Rust compilation errors Install i686 target: rustup target add i686-unknown-none
Linker errors Check that all .o files exist in build/ directory
No keyboard input Run QEMU with -serial stdio to separate serial output
GRUB multiboot error Verify boot.s contains Multiboot2 header
i686-elf-gcc not found Install cross-compiler or set CC to system GCC with -m32
"multiple definition" errors Clean build: make clean-all && make

FAQ

Q: Why build an OS from scratch?

A: For learning low-level systems programming, hardware interaction, and kernel development.

Q: Can I run Larch OS on real hardware?

A: Yes, but it is experimental. Use a spare computer or test with QEMU first.

Q: Does it support 64-bit?

A: Currently 32-bit protected mode. 64-bit long mode is planned.

Q: Can I port existing Linux programs?

A: No. Larch OS has its own syscall ABI and is not POSIX-compatible.

Q: How do I add my own programs?

A: Create an ELF executable and add it to initramfs.cpio via the Makefile.

Q: Why does it say "unknown command" when I type a valid command?

A: Ensure you typed the command exactly (lowercase). Type help to see available commands.

Q: How do I shutdown the OS?

A: Currently use the reset button in QEMU or physical power button. ACPI shutdown is planned.

Q: Does it have a GUI?

A: Not yet. Only VGA text mode. A framebuffer compositor is planned.

Contributing

Contributions are welcome! Areas that need help:

· Completing sys_fork implementation
· Adding FAT32 filesystem support
· Implementing ACPI for proper shutdown
· Porting to x86_64 long mode
· Framebuffer and GUI development

Terms of Service

Larch OS is provided "AS IS" without warranty. Use at your own risk. The authors are not responsible for any data loss, hardware damage, or other issues arising from the use of this software.

Full Terms of Service are available in the TOS.md file.

License

Larch OS is released under the BSD 3-Clause License.

Copyright (c) 2025 Larch OS Contributors

All rights reserved.

You may use, modify, and distribute this software subject to the terms of the BSD 3-Clause License. See the LICENSE file for full details.