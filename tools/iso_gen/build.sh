#!/bin/bash
set -e

ARCH=x86_64
KERNEL_BIN="build/larch.bin"
ISO_DIR="iso"
LIMINE_CFG="limine.cfg"

mkdir -p $ISO_DIR/boot
mkdir -p $ISO_DIR/boot/grub

cat > $ISO_DIR/boot/grub/grub.cfg << EOF
set timeout=5
set default=0

menuentry "Larch OS" {
    multiboot2 /boot/larch.bin
    boot
}
EOF

nasm -f elf64 kernel/larch/boot.s -o build/boot.o
rustc --target x86_64-unknown-none -C opt-level=3 -C lto \
    -Z build-std=core,alloc -Z build-std-features=compiler-builtins-mem \
    kernel/larch/paging.rs kernel/mem/pmm.rs kernel/mem/vmm.rs kernel/mem/allocator.rs \
    kernel/larch/interrupts.rs kernel/sched/scheduler.rs \
    -o build/kernel_rust.o

gcc -m64 -nostdlib -nostdinc -ffreestanding -c \
    kernel/larch/gdt.c kernel/larch/idt.c kernel/drivers/vga.c \
    kernel/drivers/serial.c kernel/sched/process.c lib/libc/string.c \
    -o build/kernel_c.o

ld -m elf_x86_64 -T linker.ld -o $KERNEL_BIN \
    build/boot.o build/kernel_rust.o build/kernel_c.o

cp $KERNEL_BIN $ISO_DIR/boot/
cp $LIMINE_CFG $ISO_DIR/boot/
cp limine/limine.sys $ISO_DIR/boot/

xorriso -as mkisofs -b boot/grub/stage2_eltorito \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    -o larch.iso $ISO_DIR/

echo "ISO built: larch.iso"