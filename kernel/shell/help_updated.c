#include <stdint.h>

extern void vga_writestring(const char* str);
extern void vga_set_color(uint8_t fg, uint8_t bg);
extern void vga_putchar(char c);

void cmd_help(void) {
    vga_writestring("\n");
    vga_set_color(0x0B, 0x00);
    vga_writestring(" Larch Commands \n");
    vga_set_color(0x0F, 0x00);
    vga_writestring("\n");
    vga_writestring("  help      - Show this help message\n");
    vga_writestring("  clear     - Clear the screen\n");
    vga_writestring("  reboot    - Reboot the system\n");
    vga_writestring("  ls        - List files in initramfs\n");
    vga_writestring("  cat <file> - Display file contents\n");
    vga_writestring("  echo <text> - Print text to screen\n");
    vga_writestring("  mem       - Show memory usage\n");
    vga_writestring("  uptime    - Show system uptime\n");
    vga_writestring("  uname     - Show OS information\n");
    vga_writestring("  hexdump <file> - Hex dump of file\n");
    vga_writestring("  date      - Show current time from RTC\n");
    vga_writestring("  top       - Show running processes\n");
    vga_writestring("\n");
    vga_set_color(0x0A, 0x00);
    vga_writestring("Navigation: Up/Down arrows for command history\n");
    vga_set_color(0x0F, 0x00);
}

void cmd_clear(void) {
    extern void vga_clear(void);
    vga_clear();
}

void cmd_reboot(void) {
    uint8_t good = 0x02;
    while (good & 0x02) {
        __asm__ volatile("inb $0x64" : "=a"(good));
    }
    __asm__ volatile("outb %0, $0x64" : : "a"(0xFE));
    for(;;);
}

void cmd_top(void) {
    extern uint32_t pit_get_tick(void);
    extern uint32_t sys_getpid(void);
    
    vga_writestring("\n");
    vga_writestring("PID  STATE     TICKS\n");
    vga_writestring("---  -----     -----\n");
    vga_writestring("  1  RUNNING   ");
    
    uint32_t ticks = pit_get_tick();
    char buf[12];
    int i = 0;
    if (ticks == 0) buf[i++] = '0';
    else {
        while (ticks) {
            buf[i++] = '0' + (ticks % 10);
            ticks /= 10;
        }
    }
    while (i--) vga_putchar(buf[i]);
    vga_writestring("\n\n");
}