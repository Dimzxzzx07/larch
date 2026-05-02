#include "multiboot.h"

uint32_t multiboot2_memory_upper = 0;
uint32_t multiboot2_memory_lower = 0;

void parse_multiboot2(uint32_t magic, uint32_t addr) {
    if (magic != 0x36D76289) return;
    
    struct multiboot2_tag* tag = (struct multiboot2_tag*)addr;
    while (tag->type != 0 && tag->size != 0) {
        if (tag->type == MULTIBOOT2_TAG_TYPE_MMAP) {
            struct multiboot2_tag_mmap* mmap = (struct multiboot2_tag_mmap*)tag;
            for (uint32_t i = 0; i < (mmap->size - sizeof(struct multiboot2_tag_mmap)) / mmap->entry_size; i++) {
                if (mmap->entries[i].type == 1) {
                    if (mmap->entries[i].addr == 0) {
                        multiboot2_memory_lower = (uint32_t)mmap->entries[i].len;
                    } else if (mmap->entries[i].addr == 0x100000) {
                        multiboot2_memory_upper = (uint32_t)mmap->entries[i].len;
                    }
                }
            }
        }
        tag = (struct multiboot2_tag*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }
}