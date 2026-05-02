#include <stdint.h>

#define MOUSE_PORT 0x60
#define MOUSE_STATUS 0x64
#define MOUSE_COMMAND 0x64

static int mouse_x = 0;
static int mouse_y = 0;
static int mouse_buttons = 0;
static int mouse_cycle = 0;
static uint8_t mouse_packet[3];

void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            uint8_t status;
            __asm__ volatile("inb $0x64, %0" : "=a"(status));
            if ((status & 1) == 1) return;
        }
    } else {
        while (timeout--) {
            uint8_t status;
            __asm__ volatile("inb $0x64, %0" : "=a"(status));
            if ((status & 2) == 0) return;
        }
    }
}

void mouse_write(uint8_t value) {
    mouse_wait(1);
    __asm__ volatile("outb %0, $0x64" : : "a"(0xD4));
    mouse_wait(1);
    __asm__ volatile("outb %0, $0x60" : : "a"(value));
}

uint8_t mouse_read(void) {
    mouse_wait(0);
    uint8_t value;
    __asm__ volatile("inb $0x60, %0" : "=a"(value));
    return value;
}

void mouse_init(void) {
    mouse_write(0xF4);
    mouse_read();
}

void mouse_handler(void) {
    uint8_t status;
    __asm__ volatile("inb $0x64, %0" : "=a"(status));
    
    if ((status & 0x20) == 0) return;
    
    uint8_t data = mouse_read();
    
    if (mouse_cycle == 0) {
        mouse_packet[0] = data;
        if ((data & 0x08) == 0) return;
        mouse_cycle = 1;
    } else if (mouse_cycle == 1) {
        mouse_packet[1] = data;
        mouse_cycle = 2;
    } else {
        mouse_packet[2] = data;
        mouse_cycle = 0;
        
        mouse_buttons = mouse_packet[0] & 0x07;
        int dx = (int8_t)mouse_packet[1];
        int dy = (int8_t)mouse_packet[2];
        
        mouse_x += dx;
        mouse_y -= dy;
        
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_x > 1024) mouse_x = 1024;
        if (mouse_y > 768) mouse_y = 768;
    }
}