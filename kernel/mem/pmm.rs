#![no_std]

use core::ptr::null_mut;
use core::sync::atomic::{AtomicUsize, Ordering};

pub struct PhysicalMemoryManager {
    bitmap: *mut u8,
    bitmap_size: usize,
    total_pages: AtomicUsize,
    free_pages: AtomicUsize,
    base_addr: usize,
}

impl PhysicalMemoryManager {
    pub const fn new() -> Self {
        PhysicalMemoryManager {
            bitmap: null_mut(),
            bitmap_size: 0,
            total_pages: AtomicUsize::new(0),
            free_pages: AtomicUsize::new(0),
            base_addr: 0,
        }
    }
    
    pub unsafe fn init(&mut self, memory_lower: usize, memory_upper: usize, bitmap_addr: usize) {
        let total_memory = memory_lower + memory_upper;
        self.total_pages = AtomicUsize::new(total_memory / 4096);
        self.free_pages = AtomicUsize::new(total_memory / 4096);
        self.base_addr = 0x100000;
        self.bitmap_size = self.total_pages.load(Ordering::SeqCst) / 8;
        self.bitmap = bitmap_addr as *mut u8;
        
        for i in 0..self.bitmap_size {
            core::ptr::write_volatile(self.bitmap.add(i), 0);
        }
        
        for addr in (0x0..0x9FC00).step_by(4096) {
            self.mark_used(addr);
        }
        for addr in (0x100000..0x200000).step_by(4096) {
            self.mark_used(addr);
        }
    }
    
    unsafe fn mark_used(&self, addr: usize) {
        let page = addr / 4096;
        let byte_idx = page / 8;
        let bit_idx = page % 8;
        if byte_idx < self.bitmap_size {
            let byte = core::ptr::read_volatile(self.bitmap.add(byte_idx));
            core::ptr::write_volatile(self.bitmap.add(byte_idx), byte | (1 << bit_idx));
            self.free_pages.fetch_sub(1, Ordering::SeqCst);
        }
    }
    
    pub unsafe fn alloc_page(&mut self) -> Option<usize> {
        for i in 0..self.bitmap_size {
            let byte = core::ptr::read_volatile(self.bitmap.add(i));
            if byte != 0xFF {
                for bit in 0..8 {
                    if (byte & (1 << bit)) == 0 {
                        let new_byte = byte | (1 << bit);
                        core::ptr::write_volatile(self.bitmap.add(i), new_byte);
                        self.free_pages.fetch_sub(1, Ordering::SeqCst);
                        let page_addr = ((i * 8 + bit) * 4096) + self.base_addr;
                        return Some(page_addr);
                    }
                }
            }
        }
        None
    }
    
    pub unsafe fn free_page(&mut self, addr: usize) {
        let page = (addr - self.base_addr) / 4096;
        let byte_idx = page / 8;
        let bit_idx = page % 8;
        if byte_idx < self.bitmap_size {
            let byte = core::ptr::read_volatile(self.bitmap.add(byte_idx));
            core::ptr::write_volatile(self.bitmap.add(byte_idx), byte & !(1 << bit_idx));
            self.free_pages.fetch_add(1, Ordering::SeqCst);
        }
    }
    
    pub fn free_pages_count(&self) -> usize {
        self.free_pages.load(Ordering::SeqCst)
    }
}