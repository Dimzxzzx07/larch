#include <stdint.h>

extern void vga_writestring(const char* str);
extern void vga_putchar(char c);
extern void vga_set_color(uint8_t fg, uint8_t bg);
extern uint32_t pit_get_tick(void);
extern uint32_t multiboot2_memory_lower;
extern uint32_t multiboot2_memory_upper;
extern void* fs_open(const char* name);
extern uint32_t fs_read(void* file, uint32_t offset, uint8_t* buf, uint32_t len);

void cmd_ls(void) {
    vga_writestring("\ninitramfs files:\n");
    vga_writestring("  bin/hello\n");
    vga_writestring("\n");
}

void cmd_echo(const char* text) {
    vga_writestring("\n");
    vga_writestring(text);
    vga_writestring("\n");
}

void cmd_mem(void) {
    uint32_t total = multiboot2_memory_lower + multiboot2_memory_upper;
    vga_writestring("\nMemory Information:\n");
    vga_writestring("  Total: ");
    uint32_t total_mb = total / (1024 * 1024);
    char digits[4];
    int i = 0;
    if (total_mb == 0) digits[i++] = '0';
    else {
        while (total_mb) {
            digits[i++] = '0' + (total_mb % 10);
            total_mb /= 10;
        }
    }
    while (i--) vga_putchar(digits[i]);
    vga_writestring(" MB\n");
    vga_writestring("  Free: unknown yet\n");
}

void cmd_uptime(void) {
    uint32_t ticks = pit_get_tick();
    uint32_t seconds = ticks / 100;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    seconds = seconds % 60;
    minutes = minutes % 60;
    
    vga_writestring("\nUptime: ");
    char buf[4];
    int i = 0;
    if (hours == 0) buf[i++] = '0';
    else {
        while (hours) {
            buf[i++] = '0' + (hours % 10);
            hours /= 10;
        }
    }
    while (i--) vga_putchar(buf[i]);
    vga_writestring("h ");
    i = 0;
    if (minutes == 0) buf[i++] = '0';
    else {
        while (minutes) {
            buf[i++] = '0' + (minutes % 10);
            minutes /= 10;
        }
    }
    while (i--) vga_putchar(buf[i]);
    vga_writestring("m ");
    i = 0;
    if (seconds == 0) buf[i++] = '0';
    else {
        while (seconds) {
            buf[i++] = '0' + (seconds % 10);
            seconds /= 10;
        }
    }
    while (i--) vga_putchar(buf[i]);
    vga_writestring("s\n");
}

void cmd_uname(void) {
    vga_writestring("\nLarch OS v1.0 (Larchwood)\n");
    vga_writestring("Kernel: Larch Kernel\n");
    vga_writestring("Arch: i686\n");
    vga_writestring("Build: " __DATE__ " " __TIME__ "\n");
}

void cmd_hexdump(const char* filename) {
    void* file = fs_open(filename);
    if (!file) {
        vga_writestring("File not found\n");
        return;
    }
    
    uint8_t buf[16];
    uint32_t offset = 0;
    uint32_t bytes;
    const char hex[] = "0123456789ABCDEF";
    
    vga_writestring("\n");
    while ((bytes = fs_read(file, offset, buf, 16)) > 0) {
        for (uint32_t i = 0; i < 8; i++) {
            if (i < bytes) {
                vga_putchar(hex[(buf[i] >> 4) & 0xF]);
                vga_putchar(hex[buf[i] & 0xF]);
            } else {
                vga_putchar(' ');
                vga_putchar(' ');
            }
            vga_putchar(' ');
        }
        vga_putchar(' ');
        for (uint32_t i = 8; i < 16; i++) {
            if (i < bytes) {
                vga_putchar(hex[(buf[i] >> 4) & 0xF]);
                vga_putchar(hex[buf[i] & 0xF]);
            } else {
                vga_putchar(' ');
                vga_putchar(' ');
            }
            vga_putchar(' ');
        }
        vga_putchar('|');
        for (uint32_t i = 0; i < bytes; i++) {
            if (buf[i] >= 32 && buf[i] <= 126) {
                vga_putchar(buf[i]);
            } else {
                vga_putchar('.');
            }
        }
        vga_writestring("|\n");
        offset += bytes;
    }
}