#![no_std]

use core::ffi::{c_char, c_void, CStr};

#[no_mangle]
pub extern "C" fn rust_allocate(size: usize) -> *mut c_void {
    unsafe {
        let layout = core::alloc::Layout::from_size_align(size, 1).unwrap();
        core::alloc::alloc(layout)
    }
}

#[no_mangle]
pub extern "C" fn rust_deallocate(ptr: *mut c_void, size: usize) {
    unsafe {
        let layout = core::alloc::Layout::from_size_align(size, 1).unwrap();
        core::alloc::dealloc(ptr as *mut u8, layout);
    }
}

#[no_mangle]
pub extern "C" fn rust_print_cstr(s: *const c_char) {
    if s.is_null() { return; }
    unsafe {
        let cstr = CStr::from_ptr(s);
        if let Ok(str_slice) = cstr.to_str() {
            for c in str_slice.chars() {
                crate::vga_write(c as *const u8, 0x0F);
            }
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_handle_page_fault(addr: usize) -> i32 {
    let mut pmm = unsafe { PMM.as_mut().unwrap() };
    if let Some(phys) = pmm.alloc_page() {
        let virt = addr & !0xFFF;
        let mut vmm = unsafe { VMM.as_mut().unwrap() };
        vmm.map_page(virt, phys, 0x07);
        1
    } else {
        0
    }
}

static mut PMM: Option<&'static mut crate::PhysicalMemoryManager> = None;
static mut VMM: Option<&'static mut crate::VirtualMemoryManager> = None;

pub unsafe fn init_bridge(pmm: &'static mut crate::PhysicalMemoryManager, vmm: &'static mut crate::VirtualMemoryManager) {
    PMM = Some(pmm);
    VMM = Some(vmm);
}