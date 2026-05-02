#include <stdint.h>

#define COM1 0x3F8

static int serial_received(void) {
    return (inb(COM1 + 5) & 1);
}

static char read_serial(void) {
    while (serial_received() == 0);
    return inb(COM1);
}

static int is_transmit_empty(void) {
    return inb(COM1 + 5) & 0x20;
}

static void write_serial(char a) {
    while (is_transmit_empty() == 0);
    outb(COM1, a);
}

void serial_init(void) {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);
}

void serial_write_string(const char* str) {
    while (*str) {
        write_serial(*str++);
    }
}