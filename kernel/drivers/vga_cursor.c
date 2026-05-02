#include <stdint.h>

static int cursor_visible = 1;
static int blink_counter = 0;
static int blink_rate = 10;

static uint16_t get_cursor_position(void) {
    uint16_t pos = 0;
    __asm__ volatile("outb %0, %1" : : "a"(0x0F), "d"(0x3D4));
    uint8_t low;
    __asm__ volatile("inb %1, %0" : "=a"(low) : "d"(0x3D5));
    __asm__ volatile("outb %0, %1" : : "a"(0x0E), "d"(0x3D4));
    uint8_t high;
    __asm__ volatile("inb %1, %0" : "=a"(high) : "d"(0x3D5));
    pos = (high << 8) | low;
    return pos;
}

static void set_cursor_position(uint16_t pos) {
    __asm__ volatile("outb %0, %1" : : "a"(0x0F), "d"(0x3D4));
    __asm__ volatile("outb %0, %1" : : "a"(pos & 0xFF), "d"(0x3D5));
    __asm__ volatile("outb %0, %1" : : "a"(0x0E), "d"(0x3D4));
    __asm__ volatile("outb %0, %1" : : "a"((pos >> 8) & 0xFF), "d"(0x3D5));
}

static void hide_cursor(void) {
    __asm__ volatile("outb %0, %1" : : "a"(0x0A), "d"(0x3D4));
    __asm__ volatile("outb %0, %1" : : "a"(0x20), "d"(0x3D5));
}

static void show_cursor(void) {
    __asm__ volatile("outb %0, %1" : : "a"(0x0A), "d"(0x3D4));
    __asm__ volatile("outb %0, %1" : : "a"(0x0E), "d"(0x3D5));
}

void update_cursor_blink(void) {
    blink_counter++;
    if (blink_counter >= blink_rate) {
        blink_counter = 0;
        cursor_visible = !cursor_visible;
        
        if (cursor_visible) {
            show_cursor();
            uint16_t pos = get_cursor_position();
            set_cursor_position(pos);
        } else {
            hide_cursor();
        }
    }
}

void init_cursor_blink(void) {
    cursor_visible = 1;
    blink_counter = 0;
    blink_rate = 10;
}