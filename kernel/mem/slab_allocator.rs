#![no_std]

use core::ptr::null_mut;
use core::mem;

struct SlabBlock {
    next: *mut SlabBlock,
    size: usize,
    free: bool,
}

pub struct SlabAllocator {
    head: *mut SlabBlock,
    total_allocated: usize,
    total_freed: usize,
}

impl SlabAllocator {
    pub const fn new() -> Self {
        SlabAllocator {
            head: null_mut(),
            total_allocated: 0,
            total_freed: 0,
        }
    }
    
    pub unsafe fn init(&mut self, heap_start: usize, heap_size: usize) {
        self.head = heap_start as *mut SlabBlock;
        (*self.head).next = null_mut();
        (*self.head).size = heap_size - mem::size_of::<SlabBlock>();
        (*self.head).free = true;
    }
    
    pub unsafe fn alloc(&mut self, size: usize, align: usize) -> *mut u8 {
        let total_size = size + mem::size_of::<SlabBlock>();
        let mut prev: *mut SlabBlock = null_mut();
        let mut current = self.head;
        
        while !current.is_null() {
            if (*current).free && (*current).size >= total_size {
                if (*current).size > total_size + mem::size_of::<SlabBlock>() {
                    let remaining = ((current as usize) + total_size) as *mut SlabBlock;
                    (*remaining).size = (*current).size - total_size;
                    (*remaining).free = true;
                    (*remaining).next = (*current).next;
                    (*current).size = total_size;
                    (*current).next = remaining;
                }
                (*current).free = false;
                self.total_allocated += (*current).size;
                return (current as usize + mem::size_of::<SlabBlock>()) as *mut u8;
            }
            prev = current;
            current = (*current).next;
        }
        null_mut()
    }
    
    pub unsafe fn dealloc(&mut self, ptr: *mut u8, _size: usize) {
        if ptr.is_null() { return; }
        
        let block = (ptr as usize - mem::size_of::<SlabBlock>()) as *mut SlabBlock;
        (*block).free = true;
        self.total_freed += (*block).size;
        
        let mut current = self.head;
        while !current.is_null() {
            let next = (*current).next;
            if !next.is_null() && (*next).free && (*current).free {
                (*current).size += (*next).size + mem::size_of::<SlabBlock>();
                (*current).next = (*next).next;
            }
            current = (*current).next;
        }
    }
    
    pub fn stats(&self) -> (usize, usize) {
        (self.total_allocated, self.total_freed)
    }
}