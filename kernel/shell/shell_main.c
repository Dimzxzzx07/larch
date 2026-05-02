#include <stdint.h>
#include <stddef.h>
#include <string.h>

extern void vga_writestring(const char* str);
extern void vga_putchar(char c);
extern void vga_set_color(uint8_t fg, uint8_t bg);
extern void shell_prompt(void);
extern void history_add(const char* cmd);
extern const char* history_prev(void);
extern const char* history_next(void);

extern void cmd_help(void);
extern void cmd_clear(void);
extern void cmd_reboot(void);
extern void cmd_ls(void);
extern void cmd_cat(const char* filename);
extern void cmd_echo(const char* text);
extern void cmd_mem(void);
extern void cmd_uptime(void);
extern void cmd_uname(void);
extern void cmd_hexdump(const char* filename);
extern void cmd_date(void);
extern void cmd_top(void);

#define MAX_COMMAND 256
#define MAX_ARGS 16

static char input_buf[MAX_COMMAND];
static int cmd_pos = 0;
static int escape_sequence = 0;

static int parse_command(char* input, char** argv) {
    int argc = 0;
    int in_word = 0;
    
    while (*input && argc < MAX_ARGS) {
        if (*input == ' ' || *input == '\t' || *input == '\n') {
            *input = 0;
            in_word = 0;
        } else {
            if (!in_word) {
                argv[argc++] = input;
                in_word = 1;
            }
        }
        input++;
    }
    return argc;
}

static char* trim_newline(char* str) {
    int len = 0;
    while (str[len]) len++;
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[len-1] = 0;
        len--;
    }
    return str;
}

void shell_process_command(char* cmdline) {
    char* argv[MAX_ARGS];
    char* trimmed = trim_newline(cmdline);
    
    if (trimmed[0] == 0) {
        shell_prompt();
        return;
    }
    
    history_add(trimmed);
    
    int argc = parse_command(trimmed, argv);
    if (argc == 0) {
        shell_prompt();
        return;
    }
    
    if (strcmp(argv[0], "help") == 0) {
        cmd_help();
    } else if (strcmp(argv[0], "clear") == 0) {
        cmd_clear();
    } else if (strcmp(argv[0], "reboot") == 0) {
        cmd_reboot();
    } else if (strcmp(argv[0], "ls") == 0) {
        cmd_ls();
    } else if (strcmp(argv[0], "cat") == 0 && argc >= 2) {
        cmd_cat(argv[1]);
    } else if (strcmp(argv[0], "echo") == 0) {
        cmd_echo(argc >= 2 ? argv[1] : "");
    } else if (strcmp(argv[0], "mem") == 0) {
        cmd_mem();
    } else if (strcmp(argv[0], "uptime") == 0) {
        cmd_uptime();
    } else if (strcmp(argv[0], "uname") == 0) {
        cmd_uname();
    } else if (strcmp(argv[0], "hexdump") == 0 && argc >= 2) {
        cmd_hexdump(argv[1]);
    } else if (strcmp(argv[0], "date") == 0) {
        cmd_date();
    } else if (strcmp(argv[0], "top") == 0) {
        cmd_top();
    } else {
        vga_set_color(0x0C, 0x00);
        vga_writestring("Unknown command: ");
        vga_writestring(argv[0]);
        vga_writestring("\n");
        vga_set_color(0x0F, 0x00);
    }
    
    shell_prompt();
}

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
        escape_sequence = 0;
    } else if (key == '\b') {
        if (cmd_pos > 0) {
            cmd_pos--;
            vga_putchar('\b');
        }
    } else if (key >= 32 && key <= 126 && cmd_pos < MAX_COMMAND - 1) {
        input_buf[cmd_pos++] = key;
        vga_putchar(key);
    }
}

void shell_init(void) {
    vga_set_color(0x0F, 0x00);
    vga_writestring("\nLarch OS Shell v1.0\n");
    vga_writestring("Type 'help' for available commands\n\n");
    shell_prompt();
}