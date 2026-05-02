#![no_std]

use core::arch::asm;

#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct Registers {
    pub ds: u32,
    pub edi: u32,
    pub esi: u32,
    pub ebp: u32,
    pub esp: u32,
    pub ebx: u32,
    pub edx: u32,
    pub ecx: u32,
    pub eax: u32,
    pub int_no: u32,
    pub err_code: u32,
    pub eip: u32,
    pub cs: u32,
    pub eflags: u32,
    pub useresp: u32,
    pub ss: u32,
}

pub type InterruptHandler = extern "C" fn(&mut Registers);

static mut HANDLERS: [Option<InterruptHandler>; 256] = [None; 256];

pub unsafe fn set_handler(int_no: u8, handler: InterruptHandler) {
    HANDLERS[int_no as usize] = Some(handler);
}

#[no_mangle]
pub extern "C" fn isr_handler(regs: &mut Registers) {
    unsafe {
        if let Some(handler) = HANDLERS[regs.int_no as usize] {
            handler(regs);
        }
    }
}

#[no_mangle]
pub extern "C" fn irq_handler(regs: &mut Registers) {
    if regs.int_no >= 40 {
        unsafe {
            asm!(
                "out dx, al",
                in("dx") 0xA0u16,
                in("al") 0x20u8,
                options(nostack)
            );
        }
    }
    unsafe {
        asm!(
            "out dx, al",
            in("dx") 0x20u16,
            in("al") 0x20u8,
            options(nostack)
        );
    }
    
    unsafe {
        if let Some(handler) = HANDLERS[regs.int_no as usize] {
            handler(regs);
        }
    }
}

pub unsafe fn enable_interrupts() {
    asm!("sti", options(nostack));
}

pub unsafe fn disable_interrupts() {
    asm!("cli", options(nostack));
}