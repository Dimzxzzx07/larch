#![no_std]
#![no_main]

use core::arch::asm;
use core::panic::PanicInfo;

const VGA_ADDR: *mut u16 = 0xB8000 as *mut u16;
const WIDTH: usize = 80;
const HEIGHT: usize = 25;

struct Terminal {
    x: usize,
    y: usize,
    color: u8,
    buffer: &'static mut [u16],
}

impl Terminal {
    fn new() -> Self {
        Terminal {
            x: 0,
            y: 0,
            color: 0x0F,
            buffer: unsafe { core::slice::from_raw_parts_mut(VGA_ADDR, WIDTH * HEIGHT) },
        }
    }
    
    fn putchar(&mut self, c: char) {
        match c {
            '\n' => {
                self.x = 0;
                self.y += 1;
            }
            '\r' => self.x = 0,
            _ => {
                if self.x >= WIDTH {
                    self.x = 0;
                    self.y += 1;
                }
                if self.y >= HEIGHT {
                    self.scroll();
                }
                self.buffer[self.y * WIDTH + self.x] = (c as u16) | ((self.color as u16) << 8);
                self.x += 1;
            }
        }
    }
    
    fn write(&mut self, s: &str) {
        for c in s.chars() {
            self.putchar(c);
        }
    }
    
    fn write_bytes(&mut self, bytes: &[u8]) {
        for &b in bytes {
            if b == b'\n' {
                self.x = 0;
                self.y += 1;
            } else if b >= 32 && b <= 126 {
                if self.x >= WIDTH {
                    self.x = 0;
                    self.y += 1;
                }
                if self.y >= HEIGHT {
                    self.scroll();
                }
                self.buffer[self.y * WIDTH + self.x] = (b as u16) | ((self.color as u16) << 8);
                self.x += 1;
            }
        }
    }
    
    fn scroll(&mut self) {
        for row in 1..HEIGHT {
            for col in 0..WIDTH {
                self.buffer[(row - 1) * WIDTH + col] = self.buffer[row * WIDTH + col];
            }
        }
        for col in 0..WIDTH {
            self.buffer[(HEIGHT - 1) * WIDTH + col] = (b' ' as u16) | ((self.color as u16) << 8);
        }
        self.y = HEIGHT - 1;
    }
    
    fn clear(&mut self) {
        for i in 0..(WIDTH * HEIGHT) {
            self.buffer[i] = (b' ' as u16) | ((self.color as u16) << 8);
        }
        self.x = 0;
        self.y = 0;
    }
    
    fn set_color(&mut self, fg: u8, bg: u8) {
        self.color = (bg << 4) | fg;
    }
    
    fn set_cursor(&mut self) {
        let pos = (self.y * WIDTH + self.x) as u16;
        unsafe {
            asm!(
                "out dx, al",
                in("dx") 0x3D4u16,
                in("al") 0x0Fu8,
                options(nostack)
            );
            asm!(
                "out dx, al",
                in("dx") 0x3D5u16,
                in("al") (pos & 0xFF) as u8,
                options(nostack)
            );
            asm!(
                "out dx, al",
                in("dx") 0x3D4u16,
                in("al") 0x0Eu8,
                options(nostack)
            );
            asm!(
                "out dx, al",
                in("dx") 0x3D5u16,
                in("al") (pos >> 8) as u8,
                options(nostack)
            );
        }
    }
}

static mut TERMINAL: Terminal = Terminal::new();

#[no_mangle]
pub extern "C" fn terminal_write(s: *const u8, len: usize) {
    unsafe {
        if !s.is_null() && len > 0 {
            let slice = core::slice::from_raw_parts(s, len);
            TERMINAL.write_bytes(slice);
            TERMINAL.set_cursor();
        }
    }
}

#[no_mangle]
pub extern "C" fn terminal_clear() {
    unsafe {
        TERMINAL.clear();
        TERMINAL.set_cursor();
    }
}

#[no_mangle]
pub extern "C" fn terminal_set_color(fg: u8, bg: u8) {
    unsafe {
        TERMINAL.set_color(fg, bg);
    }
}

#[no_mangle]
pub extern "C" fn _start() -> ! {
    unsafe {
        TERMINAL.clear();
        TERMINAL.set_color(0x0A, 0x00);
        TERMINAL.write_bytes(b"\nLarch Terminal v1.0\n");
        TERMINAL.set_color(0x0F, 0x00);
        TERMINAL.write_bytes(b"$> ");
        TERMINAL.set_cursor();
    }
    
    loop {
        unsafe {
            asm!("hlt", options(nostack));
        }
    }
}

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    unsafe {
        TERMINAL.set_color(0x0C, 0x00);
        if let Some(location) = info.location() {
            let file = location.file();
            let line = location.line();
            for b in file.bytes() {
                TERMINAL.putchar(b as char);
            }
            TERMINAL.write_bytes(b":");
            for digit in line.to_string().bytes() {
                TERMINAL.putchar(digit as char);
            }
        }
        TERMINAL.write_bytes(b"\nPANIC: ");
        if let Some(msg) = info.payload().downcast_ref::<&str>() {
            TERMINAL.write_bytes(msg.as_bytes());
        }
    }
    loop {
        unsafe {
            asm!("hlt", options(nostack));
        }
    }
}