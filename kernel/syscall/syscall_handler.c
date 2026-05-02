#include <stdint.h>

#define SYS_EXIT 1
#define SYS_READ 2
#define SYS_WRITE 3
#define SYS_OPEN 4
#define SYS_CLOSE 5
#define SYS_ALLOC 6
#define SYS_FREE 7

extern void terminal_write(const uint8_t* s, uint32_t len);
extern uint32_t sys_alloc(uint32_t size);
extern void sys_free(uint32_t ptr, uint32_t size);

struct interrupt_frame {
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t int_no;
    uint32_t err_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
};

uint32_t syscall_handler(uint32_t syscall_num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    switch (syscall_num) {
        case SYS_EXIT:
            return 0;
        case SYS_WRITE:
            terminal_write((const uint8_t*)arg1, arg2);
            return arg2;
        case SYS_ALLOC:
            return sys_alloc(arg1);
        case SYS_FREE:
            sys_free(arg1, arg2);
            return 0;
        default:
            return (uint32_t)-1;
    }
}

void syscall_int80_handler(struct interrupt_frame* frame) {
    uint32_t result = syscall_handler(frame->eax, frame->ebx, frame->ecx, frame->edx);
    frame->eax = result;
}