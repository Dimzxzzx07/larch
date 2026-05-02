#include <stdint.h>
#include <stddef.h>

extern void* fs_open(const char* name);
extern uint32_t fs_read(void* file, uint32_t offset, uint8_t* buf, uint32_t len);
extern void vga_writestring(const char* str);
extern void vga_putchar(char c);

void cmd_cat(const char* filename) {
    void* file = fs_open(filename);
    if (!file) {
        vga_writestring("File not found: ");
        vga_writestring(filename);
        vga_writestring("\n");
        return;
    }
    
    uint8_t buf[256];
    uint32_t offset = 0;
    uint32_t bytes;
    
    vga_writestring("\n");
    while ((bytes = fs_read(file, offset, buf, sizeof(buf))) > 0) {
        for (uint32_t i = 0; i < bytes; i++) {
            vga_putchar(buf[i]);
        }
        offset += bytes;
    }
    vga_writestring("\n");
}