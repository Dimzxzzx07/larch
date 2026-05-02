#ifndef LARCH_H
#define LARCH_H

#include <stdint.h>
#include <stddef.h>

#define LARCH_VERSION_MAJOR 1
#define LARCH_VERSION_MINOR 0

typedef enum {
    SYS_EXIT = 1,
    SYS_READ = 2,
    SYS_WRITE = 3,
    SYS_OPEN = 4,
    SYS_CLOSE = 5,
    SYS_ALLOC = 6,
    SYS_FREE = 7,
    SYS_AUDIO_PLAY = 100,
    SYS_AUDIO_STOP = 101,
    SYS_AUDIO_VOLUME = 102,
    SYS_GRAPHICS_RECT = 200,
    SYS_GRAPHICS_TEXT = 201,
} syscall_num_t;

static inline long syscall(long num, long arg1, long arg2, long arg3) {
    long ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
        : "memory"
    );
    return ret;
}

void* larch_alloc(size_t size);
void larch_free(void* ptr);
void larch_print(const char* str);
int larch_open(const char* path, int flags);
int larch_read(int fd, void* buf, size_t count);
int larch_write(int fd, const void* buf, size_t count);
void larch_exit(int code);

#endif