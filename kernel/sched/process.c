#include <stdint.h>
#include <stddef.h>

typedef struct process {
    uint32_t pid;
    uint32_t state;
    uint32_t* stack;
    struct process* next;
} process_t;

static process_t* current = NULL;
static process_t* ready_queue = NULL;

uint32_t create_process(void* entry) {
    static uint32_t next_pid = 1;
    process_t* proc = (process_t*)0x100000;
    proc->pid = next_pid++;
    proc->state = 1;
    proc->next = ready_queue;
    ready_queue = proc;
    return proc->pid;
}

void schedule(void) {
    if (current) {
        current = current->next;
    } else {
        current = ready_queue;
    }
    if (!current) return;
}