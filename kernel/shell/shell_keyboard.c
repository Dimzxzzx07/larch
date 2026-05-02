#include <stdint.h>
#include <string.h>

static char input_buf[256];
static int cmd_pos = 0;
static int history_index = 0;
static int escape_sequence = 0;

extern void vga_putchar(char c);
extern void vga_writestring(const char* str);
extern void shell_prompt(void);
extern void shell_process_command(char* cmdline);
extern void history_add(const char* cmd);
extern const char* history_prev(void);
extern const char* history_next(void);

void shell_handle_key(char key) {
    if (escape_sequence == 1) {
        if (key == '[') {
            escape_sequence = 2;
            return;
        }
        escape_sequence = 0;
    }
    
    if (escape_sequence == 2) {
        escape_sequence = 0;
        if (key == 'A') {
            const char* prev = history_prev();
            if (prev[0] != 0) {
                for (int i = 0; i < cmd_pos; i++) {
                    vga_putchar('\b');
                    vga_putchar(' ');
                    vga_putchar('\b');
                }
                strcpy(input_buf, prev);
                cmd_pos = strlen(prev);
                vga_writestring(input_buf);
            }
        } else if (key == 'B') {
            const char* next = history_next();
            for (int i = 0; i < cmd_pos; i++) {
                vga_putchar('\b');
                vga_putchar(' ');
                vga_putchar('\b');
            }
            if (next[0] != 0) {
                strcpy(input_buf, next);
                cmd_pos = strlen(next);
                vga_writestring(input_buf);
            } else {
                input_buf[0] = 0;
                cmd_pos = 0;
            }
        }
        return;
    }
    
    if (key == 0x1B) {
        escape_sequence = 1;
        return;
    }
    
    if (key == '\n') {
        input_buf[cmd_pos] = 0;
        vga_putchar('\n');
        shell_process_command(input_buf);
        cmd_pos = 0;
        history_index = 0;
        escape_sequence = 0;
    } else if (key == '\b') {
        if (cmd_pos > 0) {
            cmd_pos--;
            vga_putchar('\b');
        }
    } else if (key >= 32 && key <= 126 && cmd_pos < 255) {
        input_buf[cmd_pos++] = key;
        vga_putchar(key);
    }
}

void terminal_readline(char* buf, int max_len) {
    int pos = 0;
    int escape = 0;
    
    while (1) {
        uint8_t scancode;
        __asm__ volatile("inb $0x60, %0" : "=a"(scancode));
        
        if (scancode & 0x80) continue;
        
        char ascii = 0;
        switch (scancode) {
            case 0x1C: ascii = '\n'; break;
            case 0x0E: ascii = '\b'; break;
            case 0x01: ascii = 0x1B; break;
            case 0x02: ascii = '1'; break;
            case 0x03: ascii = '2'; break;
            case 0x04: ascii = '3'; break;
            case 0x05: ascii = '4'; break;
            case 0x06: ascii = '5'; break;
            case 0x07: ascii = '6'; break;
            case 0x08: ascii = '7'; break;
            case 0x09: ascii = '8'; break;
            case 0x0A: ascii = '9'; break;
            case 0x0B: ascii = '0'; break;
            case 0x10: ascii = 'q'; break;
            case 0x11: ascii = 'w'; break;
            case 0x12: ascii = 'e'; break;
            case 0x13: ascii = 'r'; break;
            case 0x14: ascii = 't'; break;
            case 0x15: ascii = 'y'; break;
            case 0x16: ascii = 'u'; break;
            case 0x17: ascii = 'i'; break;
            case 0x18: ascii = 'o'; break;
            case 0x19: ascii = 'p'; break;
            case 0x1E: ascii = 'a'; break;
            case 0x1F: ascii = 's'; break;
            case 0x20: ascii = 'd'; break;
            case 0x21: ascii = 'f'; break;
            case 0x22: ascii = 'g'; break;
            case 0x23: ascii = 'h'; break;
            case 0x24: ascii = 'j'; break;
            case 0x25: ascii = 'k'; break;
            case 0x26: ascii = 'l'; break;
            case 0x2C: ascii = 'z'; break;
            case 0x2D: ascii = 'x'; break;
            case 0x2E: ascii = 'c'; break;
            case 0x2F: ascii = 'v'; break;
            case 0x30: ascii = 'b'; break;
            case 0x31: ascii = 'n'; break;
            case 0x32: ascii = 'm'; break;
            case 0x39: ascii = ' '; break;
        }
        
        if (ascii == '\n') {
            buf[pos] = 0;
            vga_putchar('\n');
            return;
        } else if (ascii == '\b' && pos > 0) {
            pos--;
            vga_putchar('\b');
        } else if (ascii >= 32 && ascii <= 126 && pos < max_len - 1) {
            buf[pos++] = ascii;
            vga_putchar(ascii);
        } else if (ascii == 0x1B) {
            uint8_t seq[2];
            __asm__ volatile("inb $0x60, %0" : "=a"(seq[0]));
            __asm__ volatile("inb $0x60, %0" : "=a"(seq[1]));
            if (seq[0] == 0x1C && seq[1] == 0x4D) {
                __asm__ volatile("inb $0x60, %0" : "=a"(seq[0]));
                if (seq[0] == 0x48) {
                    const char* prev = history_prev();
                    if (prev[0]) {
                        for (int i = 0; i < pos; i++) vga_putchar('\b');
                        strcpy(buf, prev);
                        pos = strlen(prev);
                        vga_writestring(buf);
                    }
                } else if (seq[0] == 0x50) {
                    const char* next = history_next();
                    for (int i = 0; i < pos; i++) vga_putchar('\b');
                    if (next[0]) {
                        strcpy(buf, next);
                        pos = strlen(next);
                        vga_writestring(buf);
                    } else {
                        buf[0] = 0;
                        pos = 0;
                    }
                }
            }
        }
    }
}