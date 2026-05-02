#![no_std]

use core::alloc::{GlobalAlloc, Layout};
use core::ptr::null_mut;

struct ListNode {
    size: usize,
    next: Option<&'static mut ListNode>,
}

pub struct BumpAllocator {
    heap_start: usize,
    heap_end: usize,
    next: usize,
}

impl BumpAllocator {
    pub const fn new() -> Self {
        BumpAllocator {
            heap_start: 0,
            heap_end: 0,
            next: 0,
        }
    }
    
    pub unsafe fn init(&mut self, heap_start: usize, heap_size: usize) {
        self.heap_start = heap_start;
        self.heap_end = heap_start + heap_size;
        self.next = heap_start;
    }
    
    unsafe fn alloc(&mut self, size: usize, align: usize) -> *mut u8 {
        let align_mask = align - 1;
        let current = (self.next + align_mask) & !align_mask;
        
        if current + size > self.heap_end {
            null_mut()
        } else {
            self.next = current + size;
            current as *mut u8
        }
    }
    
    unsafe fn dealloc(&mut self, _ptr: *mut u8, _size: usize) {}
}

unsafe impl GlobalAlloc for BumpAllocator {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        let allocator = &mut *(self as *const _ as *mut BumpAllocator);
        allocator.alloc(layout.size(), layout.align())
    }
    
    unsafe fn dealloc(&self, ptr: *mut u8, layout: Layout) {
        let allocator = &mut *(self as *const _ as *mut BumpAllocator);
        allocator.dealloc(ptr, layout.size())
    }
}

#[global_allocator]
static ALLOCATOR: BumpAllocator = BumpAllocator::new();