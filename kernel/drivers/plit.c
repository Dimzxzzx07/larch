#include <stdint.h>

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_FREQUENCY 1193182
#define TARGET_HZ 100

static uint32_t tick_count = 0;

extern void schedule(void);
extern void update_cursor_blink(void);

void pit_init(void) {
    uint32_t divisor = PIT_FREQUENCY / TARGET_HZ;
    
    __asm__ volatile("cli");
    
    __asm__ volatile("outb %0, %1" : : "a"(0x36), "d"(PIT_COMMAND));
    __asm__ volatile("outb %0, %1" : : "a"(divisor & 0xFF), "d"(PIT_CHANNEL0));
    __asm__ volatile("outb %0, %1" : : "a"((divisor >> 8) & 0xFF), "d"(PIT_CHANNEL0));
    
    __asm__ volatile("sti");
}

uint32_t pit_get_tick(void) {
    return tick_count;
}

void pit_handler(void) {
    tick_count++;
    update_cursor_blink();
    schedule();
}