#include <stdint.h>
#include <stddef.h>

typedef struct {
    char name[64];
    uint32_t size;
   uint8_t* data;
    uint32_t type;
} file_t;

static file_t files[32];
static uint32_t file_count = 0;

extern uint8_t _binary_initramfs_start[];
extern uint8_t _binary_initramfs_end[];

void* fs_open(const char* name) {
    for (uint32_t i = 0; i < file_count; i++) {
        uint32_t j = 0;
        while (files[i].name[j] && name[j] && files[i].name[j] == name[j]) j++;
        if (files[i].name[j] == 0 && name[j] == 0) {
            return (void*)(uintptr_t)(i + 1);
        }
    }
    return 0;
}

uint32_t fs_read(void* file, uint32_t offset, uint8_t* buf, uint32_t len) {
    uint32_t idx = (uint32_t)(uintptr_t)file - 1;
    if (idx >= file_count) return 0;
    
    file_t* f = &files[idx];
    if (offset >= f->size) return 0;
    
    uint32_t to_read = len;
    if (offset + to_read > f->size) to_read = f->size - offset;
    
    for (uint32_t i = 0; i < to_read; i++) {
        buf[i] = f->data[offset + i];
    }
    return to_read;
}

uint32_t fs_size(void* file) {
    uint32_t idx = (uint32_t)(uintptr_t)file - 1;
    if (idx >= file_count) return 0;
    return files[idx].size;
}