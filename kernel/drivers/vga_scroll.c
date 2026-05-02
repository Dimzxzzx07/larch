#include <stdint.h>

#define VGA_ADDR 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t* vga_buffer = (uint16_t*)VGA_ADDR;
static int cursor_x = 0;
static int cursor_y = 0;
static uint8_t current_color = 0x0F;

void vga_set_color(uint8_t fg, uint8_t bg) {
    current_color = (bg << 4) | fg;
}

void vga_scroll(void) {
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[(y - 1) * VGA_WIDTH + x] = vga_buffer[y * VGA_WIDTH + x];
        }
    }
    
    for (int x = 0; x < VGA_WIDTH; x++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (' ' | (current_color << 8));
    }
    
    cursor_y = VGA_HEIGHT - 1;
}

void vga_putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (' ' | (current_color << 8));
        }
    } else {
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (c | (current_color << 8));
        cursor_x++;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= VGA_HEIGHT) {
        vga_scroll();
    }
    
    uint16_t pos = cursor_y * VGA_WIDTH + cursor_x;
    __asm__ volatile("outb %0, %1" : : "a"(0x0F), "d"(0x3D4));
    __asm__ volatile("outb %0, %1" : : "a"(pos & 0xFF), "d"(0x3D5));
    __asm__ volatile("outb %0, %1" : : "a"(0x0E), "d"(0x3D4));
    __asm__ volatile("outb %0, %1" : : "a"(pos >> 8), "d"(0x3D5));
}

void vga_writestring(const char* str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (' ' | (current_color << 8));
    }
    cursor_x = 0;
    cursor_y = 0;
}

void vga_init(void) {
    vga_clear();
    vga_set_color(0x0F, 0x00);
}