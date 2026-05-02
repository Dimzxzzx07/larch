#include <stdint.h>

static const char* boot_splash = 
"\n"
"  _               _     \n"
" | |             | |    \n"
" | |     __ _ ___| |__  \n"
" | |    / _` / __| '_ \\ \n"
" | |___| (_| \\__ \\ | | |\n"
" |______\\__,_|___/_| |_|\n"
"                         \n"
"  Larch OS v1.0\n"
"  The flexible operating system\n"
"\n";

void show_boot_splash(void) {
    extern void vga_writestring(const char* str);
    extern void vga_set_color(uint8_t fg, uint8_t bg);
    
    vga_set_color(0x0A, 0x00);
    vga_writestring(boot_splash);
    vga_set_color(0x0F, 0x00);
    
    for (int i = 0; i < 10000000; i++);
}

void shell_prompt(void) {
    extern void vga_writestring(const char* str);
    extern void vga_set_color(uint8_t fg, uint8_t bg);
    
    vga_set_color(0x0C, 0x00);
    vga_writestring("[");
    vga_set_color(0x0B, 0x00);
    vga_writestring("larch");
    vga_set_color(0x0C, 0x00);
    vga_writestring("]");
    vga_set_color(0x0A, 0x00);
    vga_writestring(" $> ");
    vga_set_color(0x0F, 0x00);
}