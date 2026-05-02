#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t magic[4];
    uint8_t bits;
    uint8_t endian;
    uint8_t version;
    uint8_t osabi;
    uint8_t abiversion;
    uint8_t pad[7];
    uint16_t type;
    uint16_t machine;
    uint32_t version2;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} elf32_header_t;

typedef struct {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
} elf32_phdr_t;

#define PT_LOAD 1

extern void* sys_alloc_page(void);
extern void sys_map_page(void* virt, void* phys, uint32_t flags);
extern void* memcpy(void* dest, const void* src, size_t n);

static void* alloc_page_at(void* vaddr) {
    void* phys = sys_alloc_page();
    if (!phys) return 0;
    sys_map_page(vaddr, phys, 0x07);
    return vaddr;
}

void* elf_load(const uint8_t* elf_data) {
    elf32_header_t* header = (elf32_header_t*)elf_data;
    
    if (header->magic[0] != 0x7F || header->magic[1] != 'E' || 
        header->magic[2] != 'L' || header->magic[3] != 'F') {
        return 0;
    }
    
    if (header->type != 2) return 0;
    
    for (uint32_t i = 0; i < header->phnum; i++) {
        elf32_phdr_t* phdr = (elf32_phdr_t*)(elf_data + header->phoff + i * header->phentsize);
        if (phdr->type == PT_LOAD && phdr->memsz > 0) {
            uint32_t start_page = phdr->vaddr & ~0xFFF;
            uint32_t end_page = (phdr->vaddr + phdr->memsz + 0xFFF) & ~0xFFF;
            
            for (uint32_t page = start_page; page < end_page; page += 4096) {
                if (!alloc_page_at((void*)page)) {
                    return 0;
                }
            }
            
            if (phdr->filesz > 0) {
                memcpy((void*)phdr->vaddr, elf_data + phdr->offset, phdr->filesz);
            }
            
            if (phdr->memsz > phdr->filesz) {
                uint8_t* bss_start = (uint8_t*)(phdr->vaddr + phdr->filesz);
                uint32_t bss_size = phdr->memsz - phdr->filesz;
                for (uint32_t j = 0; j < bss_size; j++) {
                    bss_start[j] = 0;
                }
            }
        }
    }
    
    return (void*)(uintptr_t)header->entry;
}

uint32_t exec_user_program(const uint8_t* elf_data) {
    void* entry = elf_load(elf_data);
    if (!entry) return 0;
    
    uint32_t pid;
    __asm__ volatile(
        "mov %1, %%eax\n"
        "int $0x80\n"
        "mov %%eax, %0"
        : "=r"(pid)
        : "r"(entry)
        : "eax", "memory"
    );
    return pid;
}