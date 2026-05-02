.section .multiboot2
.align 8
    .long 0xE85250D6
    .long 0
    .long header_end - header_start
    .long -(0xE85250D6 + 0 + (header_end - header_start))

header_start:
    .word 0
    .word 0
    .long 8
header_end:

.section .text
.global _start
.type _start, @function
_start:
    mov $stack_top, %esp
    push %ebx
    push %eax
    call kernel_main
    cli
    hlt

.section .bss
.align 16
.space 16384
stack_top: