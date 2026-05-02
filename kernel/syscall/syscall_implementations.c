#include <stdint.h>

extern uint32_t sys_alloc(uint32_t size);
extern void sys_free(uint32_t ptr, uint32_t size);
extern uint32_t pit_get_tick(void);
extern void schedule(void);

static uint32_t next_pid = 1;
static uint32_t current_pid = 1;

uint32_t sys_fork(void) {
    uint32_t new_pid = next_pid++;
    return new_pid;
}

uint32_t sys_exec(const char* path, char* const argv[]) {
    (void)path;
    (void)argv;
    return -1;
}

uint32_t sys_getpid(void) {
    return current_pid;
}

void sys_sleep(uint32_t ticks) {
    uint32_t start = pit_get_tick();
    while ((pit_get_tick() - start) < ticks) {
        schedule();
    }
}

uint32_t sys_waitpid(uint32_t pid) {
    (void)pid;
    return 0;
}

uint32_t sys_getticks(void) {
    return pit_get_tick();
}

uint32_t sys_alloc_wrapper(uint32_t size) {
    return sys_alloc(size);
}

void sys_free_wrapper(uint32_t ptr, uint32_t size) {
    sys_free(ptr, size);
}