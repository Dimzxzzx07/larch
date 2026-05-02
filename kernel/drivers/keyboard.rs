#![no_std]

use core::arch::asm;

const KEYBOARD_PORT: u16 = 0x60;
const KEYBOARD_STATUS: u16 = 0x64;

static SCANCODE_TO_ASCII: [u8; 128] = [
    0, 0, b'1', b'2', b'3', b'4', b'5', b'6',
    b'7', b'8', b'9', b'0', b'-', b'=', 0, 0,
    b'q', b'w', b'e', b'r', b't', b'y', b'u', b'i',
    b'o', b'p', b'[', b']', 0, 0, b'a', b's',
    b'd', b'f', b'g', b'h', b'j', b'k', b'l', b';',
    b'\'', b'`', 0, b'\\', b'z', b'x', b'c', b'v',
    b'b', b'n', b'm', b',', b'.', b'/', 0, b'*',
    0, b' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
];

pub struct Keyboard {
    shift: bool,
    caps: bool,
    ctrl: bool,
    alt: bool,
}

impl Keyboard {
    pub const fn new() -> Self {
        Keyboard {
            shift: false,
            caps: false,
            ctrl: false,
            alt: false,
        }
    }
    
    pub unsafe fn read_scancode(&mut self) -> Option<u8> {
        let status: u8;
        asm!(
            "in al, dx",
            out("al") status,
            in("dx") KEYBOARD_STATUS,
            options(nostack, nomem)
        );
        
        if status & 1 == 1 {
            let scancode: u8;
            asm!(
                "in al, dx",
                out("al") scancode,
                in("dx") KEYBOARD_PORT,
                options(nostack, nomem)
            );
            Some(scancode)
        } else {
            None
        }
    }
    
    pub fn handle_scancode(&mut self, scancode: u8) -> Option<char> {
        match scancode {
            0x1D => {
                self.ctrl = true;
                return None;
            }
            0x9D => {
                self.ctrl = false;
                return None;
            }
            0x38 => {
                self.alt = true;
                return None;
            }
            0xB8 => {
                self.alt = false;
                return None;
            }
            0x2A | 0x36 => {
                self.shift = true;
                return None;
            }
            0xAA | 0xB6 => {
                self.shift = false;
                return None;
            }
            0x3A => {
                self.caps = !self.caps;
                return None;
            }
            _ => {}
        }
        
        if scancode & 0x80 != 0 {
            return None;
        }
        
        let mut ascii = SCANCODE_TO_ASCII[scancode as usize];
        if ascii != 0 {
            let shift_active = self.shift ^ self.caps;
            if shift_active && ascii >= b'a' && ascii <= b'z' {
                ascii = ascii - 32;
            } else if shift_active && ascii >= b'A' && ascii <= b'Z' {
                ascii = ascii + 32;
            }
            
            if self.ctrl && ascii >= 0x60 {
                return Some(((ascii - 0x60) as char));
            }
            return Some(ascii as char);
        }
        None
    }
    
    pub fn reset_modifiers(&mut self) {
        self.shift = false;
        self.ctrl = false;
        self.alt = false;
    }
}