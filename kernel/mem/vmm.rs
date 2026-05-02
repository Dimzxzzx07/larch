#![no_std]

use core::arch::asm;
use super::pmm::PhysicalMemoryManager;

#[derive(Clone, Copy)]
pub struct VirtualAddress(pub u32);

#[derive(Clone, Copy)]
pub struct PhysicalAddress(pub u32);

pub struct VirtualMemoryManager {
    page_directory: *mut u32,
    pmm: &'static mut PhysicalMemoryManager,
}

impl VirtualMemoryManager {
    pub fn new(pmm: &'static mut PhysicalMemoryManager) -> Self {
        VirtualMemoryManager {
            page_directory: core::ptr::null_mut(),
            pmm,
        }
    }
    
    pub unsafe fn map_page(&mut self, virt: VirtualAddress, phys: PhysicalAddress, flags: u32) {
        let pd_index = ((virt.0 >> 22) & 0x3FF) as usize;
        let pt_index = ((virt.0 >> 12) & 0x3FF) as usize;
        
        let page_table = if (*(self.page_directory).add(pd_index) & 1) != 0 {
            (*(self.page_directory).add(pd_index) & !0xFFF) as *mut u32
        } else {
            let new_pt = self.pmm.alloc_page().unwrap() as *mut u32;
            *(self.page_directory).add(pd_index) = (new_pt as u32) | 0x7;
            new_pt
        };
        
        *(page_table.add(pt_index)) = (phys.0) | flags | 1;
    }
    
    pub unsafe fn unmap_page(&mut self, virt: VirtualAddress) {
        let pd_index = ((virt.0 >> 22) & 0x3FF) as usize;
        let pt_index = ((virt.0 >> 12) & 0x3FF) as usize;
        
        if (*(self.page_directory).add(pd_index) & 1) != 0 {
            let page_table = (*(self.page_directory).add(pd_index) & !0xFFF) as *mut u32;
            *(page_table.add(pt_index)) = 0;
        }
    }
    
    pub unsafe fn switch_directory(&mut self, directory: *mut u32) {
        asm!(
            "mov cr3, {}",
            in(reg) directory,
            options(nostack)
        );
        self.page_directory = directory;
    }
    
    pub unsafe fn get_cr3(&self) -> u32 {
        let cr3: u32;
        asm!(
            "mov {}, cr3",
            out(reg) cr3,
            options(nostack)
        );
        cr3
    }
    
    pub unsafe fn tlb_flush(&mut self, virt: VirtualAddress) {
        asm!(
            "invlpg [{}]",
            in(reg) virt.0,
            options(nostack)
        );
    }
}