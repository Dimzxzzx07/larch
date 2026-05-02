#include <stdint.h>
#include <stdarg.h>

extern void vga_putchar(char c);
extern void vga_writestring(const char* str);

static void print_hex(uint32_t num, int digits) {
    const char hex[] = "0123456789ABCDEF";
    char buf[11];
    int i = 0;
    
    for (int j = digits - 1; j >= 0; j--) {
        buf[i++] = hex[(num >> (j * 4)) & 0xF];
    }
    buf[i] = 0;
    vga_writestring(buf);
}

static void print_dec(uint32_t num) {
    char buf[12];
    int i = 0;
    
    if (num == 0) {
        vga_putchar('0');
        return;
    }
    
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        vga_putchar(buf[--i]);
    }
}

void kprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's':
                    vga_writestring(va_arg(args, const char*));
                    break;
                case 'd':
                case 'i':
                    print_dec(va_arg(args, uint32_t));
                    break;
                case 'x':
                    print_hex(va_arg(args, uint32_t), 8);
                    break;
                case 'X':
                    print_hex(va_arg(args, uint32_t), 8);
                    break;
                case 'c':
                    vga_putchar(va_arg(args, int));
                    break;
                case '%':
                    vga_putchar('%');
                    break;
                default:
                    vga_putchar('%');
                    vga_putchar(*fmt);
            }
        } else {
            vga_putchar(*fmt);
        }
        fmt++;
    }
    
    va_end(args);
}

void printf_init(void) {
    kprintf("[INFO] printf initialized\n");
}