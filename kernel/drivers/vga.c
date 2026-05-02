#include <stdint.h>

#define VGA_ADDR 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t* vga_buffer = (uint16_t*)VGA_ADDR;
static int cursor_x = 0;
static int cursor_y = 0;

uint8_t vga_color(uint8_t fg, uint8_t bg) {
    return (bg << 4) | fg;
}

void vga_putchar(char c, uint8_t color) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        vga_buffer[cursor_y * VGA_WIDTH + cursor_x] = (uint16_t)c | ((uint16_t)color << 8);
        cursor_x++;
    }
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= VGA_HEIGHT) {
        cursor_y = 0;
    }
}

void vga_write(const char* str, uint8_t color) {
    while (*str) {
        vga_putchar(*str++, color);
    }
}