#![no_std]

use core::ptr::null_mut;
use core::mem;

#[repr(C)]
#[derive(Clone, Copy)]
pub struct PageTableEntry {
    pub value: u64,
}

impl PageTableEntry {
    pub const fn new() -> Self {
        PageTableEntry { value: 0 }
    }
    
    pub fn set_address(&mut self, addr: u64) {
        self.value = (self.value & !0x000FFFFFFFFFF000) | (addr & 0x000FFFFFFFFFF000);
    }
    
    pub fn set_present(&mut self, present: bool) {
        if present {
            self.value |= 1;
        } else {
            self.value &= !1;
        }
    }
    
    pub fn set_writable(&mut self, writable: bool) {
        if writable {
            self.value |= 1 << 1;
        } else {
            self.value &= !(1 << 1);
        }
    }
    
    pub fn set_user(&mut self, user: bool) {
        if user {
            self.value |= 1 << 2;
        } else {
            self.value &= !(1 << 2);
        }
    }
    
    pub fn is_present(&self) -> bool {
        (self.value & 1) != 0
    }
    
    pub fn physical_address(&self) -> u64 {
        self.value & 0x000FFFFFFFFFF000
    }
}

static mut PML4: [PageTableEntry; 512] = [PageTableEntry::new(); 512];
static mut PDPT: [PageTableEntry; 512] = [PageTableEntry::new(); 512];
static mut PD: [[PageTableEntry; 512]; 4] = [[PageTableEntry::new(); 512]; 4];
static mut PAGE_TABLES_INITIALIZED: bool = false;

pub unsafe fn init_paging(pmm: &mut super::pmm::PhysicalMemoryManager) {
    if PAGE_TABLES_INITIALIZED {
        return;
    }
    
    for i in 0..512 {
        PD[0][i].set_present(true);
        PD[0][i].set_writable(true);
        PD[0][i].set_address((i * 4096) as u64);
    }
    
    for table_idx in 1..4 {
        for i in 0..512 {
            PD[table_idx][i].set_present(false);
            PD[table_idx][i].set_address(0);
        }
    }
    
    PDPT[0].set_present(true);
    PDPT[0].set_writable(true);
    PDPT[0].set_address(&PD[0] as *const _ as u64);
    
    for i in 1..4 {
        PDPT[i].set_present(false);
        PDPT[i].set_address(0);
    }
    
    PML4[0].set_present(true);
    PML4[0].set_writable(true);
    PML4[0].set_address(&PDPT as *const _ as u64);
    
    for i in 1..512 {
        PML4[i].set_present(false);
        PML4[i].set_address(0);
    }
    
    let cr3 = &PML4 as *const _ as u64;
    asm!("mov {}, cr3", in(reg) cr3, options(nostack));
    
    PAGE_TABLES_INITIALIZED = true;
}

pub unsafe fn map_page(virt_addr: u64, phys_addr: u64, flags: u64, pmm: &mut super::pmm::PhysicalMemoryManager) {
    let pml4_idx = ((virt_addr >> 39) & 0x1FF) as usize;
    let pdpt_idx = ((virt_addr >> 30) & 0x1FF) as usize;
    let pd_idx = ((virt_addr >> 21) & 0x1FF) as usize;
    let pt_idx = ((virt_addr >> 12) & 0x1FF) as usize;
    
    if !PML4[pml4_idx].is_present() {
        let new_pdpt = pmm.alloc_page().unwrap() as u64;
        PML4[pml4_idx].set_address(new_pdpt);
        PML4[pml4_idx].set_present(true);
        PML4[pml4_idx].set_writable(true);
        PML4[pml4_idx].set_user((flags & 4) != 0);
    }
    
    let pdpt_addr = PML4[pml4_idx].physical_address();
    let pdpt = pdpt_addr as *mut PageTableEntry;
    
    if !(*pdpt.add(pdpt_idx)).is_present() {
        let new_pd = pmm.alloc_page().unwrap() as u64;
        (*pdpt.add(pdpt_idx)).set_address(new_pd);
        (*pdpt.add(pdpt_idx)).set_present(true);
        (*pdpt.add(pdpt_idx)).set_writable(true);
        (*pdpt.add(pdpt_idx)).set_user((flags & 4) != 0);
    }
    
    let pd_addr = (*pdpt.add(pdpt_idx)).physical_address();
    let pd = pd_addr as *mut PageTableEntry;
    
    if !(*pd.add(pd_idx)).is_present() {
        let new_pt = pmm.alloc_page().unwrap() as u64;
        (*pd.add(pd_idx)).set_address(new_pt);
        (*pd.add(pd_idx)).set_present(true);
        (*pd.add(pd_idx)).set_writable(true);
        (*pd.add(pd_idx)).set_user((flags & 4) != 0);
    }
    
    let pt_addr = (*pd.add(pd_idx)).physical_address();
    let pt = pt_addr as *mut PageTableEntry;
    
    (*pt.add(pt_idx)).set_address(phys_addr & 0x000FFFFFFFFFF000);
    (*pt.add(pt_idx)).set_present(true);
    (*pt.add(pt_idx)).set_writable((flags & 2) != 0);
    (*pt.add(pt_idx)).set_user((flags & 4) != 0);
}