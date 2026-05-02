// kernel/kernel_main.c (updated)
#include <stdint.h>
#include <stddef.h>
#include "larch/multiboot.h"

extern void gdt_init(void);
extern void idt_init(void);
extern void pmm_init(uint32_t mmap_lower, uint32_t mmap_upper, uint32_t bitmap_addr);
extern void tss_init(uint32_t kernel_stack);
extern void initramfs_init(void);
extern void pit_init(void);
extern void scheduler_init(void);
extern void keyboard_init(void);
extern void vga_init(void);
extern void printf_init(void);
extern void terminal_writestring(const char* str);
extern void shell_init(void);
extern void parse_multiboot2(uint32_t magic, uint32_t addr);

void kernel_panic(const char* msg, const char* file, int line) {
    terminal_writestring("\n\n KERNEL PANIC \n");
    terminal_writestring("Message: ");
    terminal_writestring(msg);
    terminal_writestring("\nLocation: ");
    terminal_writestring(file);
    terminal_writestring(":");
    for (int i = 10000; i > 0; i--);
    terminal_writestring("\nSystem halted.\n");
    for(;;) {
        __asm__ volatile("hlt");
    }
}

void kernel_main(uint32_t magic, uint32_t addr) {
    vga_init();
    terminal_writestring("Larch OS v1.0\n");
    
    terminal_writestring("Parsing multiboot...\n");
    parse_multiboot2(magic, addr);
    terminal_writestring("OK\n");
    
    terminal_writestring("Initializing GDT...\n");
    gdt_init();
    terminal_writestring("OK\n");
    
    terminal_writestring("Initializing IDT...\n");
    idt_init();
    terminal_writestring("OK\n");
    
    terminal_writestring("Initializing PMM...\n");
    uint32_t bitmap_addr = 0x200000;
    pmm_init(multiboot2_memory_lower, multiboot2_memory_upper, bitmap_addr);
    terminal_writestring("OK\n");
    
    terminal_writestring("Initializing TSS...\n");
    tss_init(0x300000);
    terminal_writestring("OK\n");
    
    terminal_writestring("Initializing printf...\n");
    printf_init();
    terminal_writestring("OK\n");
    
    terminal_writestring("Initializing PIT...\n");
    pit_init();
    terminal_writestring("OK\n");
    
    terminal_writestring("Initializing initramfs...\n");
    initramfs_init();
    terminal_writestring("OK\n");
    
    terminal_writestring("Initializing keyboard...\n");
    keyboard_init();
    terminal_writestring("OK\n");
    
    terminal_writestring("Starting scheduler...\n");
    scheduler_init();
    terminal_writestring("OK\n");
    
    terminal_writestring("Initializing shell...\n");
    shell_init();
    terminal_writestring("OK\n");
    
    terminal_writestring("Larch OS is ready.\n");
    
    __asm__ volatile("sti");
    
    for(;;) {
        __asm__ volatile("hlt");
    }
}