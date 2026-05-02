CC     = i686-elf-gcc
CXX    = i686-elf-g++
AS     = i686-elf-as
LD     = i686-elf-ld
RUSTC  = rustc

CFLAGS   = -std=c99 -ffreestanding -nostdlib -nostdinc -fno-builtin \
           -fno-stack-protector -O2 -Wall -Wextra \
           -I./kernel -I./kernel/larch
CXXFLAGS = -std=c++11 -ffreestanding -nostdlib -fno-rtti -fno-exceptions -O2
RUSTFLAGS = --target i686-unknown-none -C opt-level=3 -C lto \
            -Z build-std=core,alloc
ASFLAGS  = -felf32
LDFLAGS  = -T linker.ld -ffreestanding -nostdlib

OBJS = \
	build/boot.o              \
	build/multiboot.o         \
	\
	build/gdt_full.o          \
	build/idt.o               \
	\
	build/pmm.o               \
	build/vmm.o               \
	\
	build/pit.o               \
	build/vga_scroll.o        \
	build/vga_cursor.o        \
	build/rtc.o               \
	build/ps2_mouse.o         \
	build/serial.o            \
	build/keyboard.o          \
	\
	build/kprintf.o           \
	build/initramfs.o         \
	build/vfs.o               \
	build/elf_loader.o        \
	\
	build/syscall_handler.o   \
	build/syscall_implementations.o \
	\
	build/shell_main.o        \
	build/builtin_commands.o  \
	build/cmd_cat.o           \
	build/help_updated.o      \
	build/command_history.o   \
	build/colored_shell.o     \
	\
	build/context_switch.o    \
	build/kernel_main.o

all: larch.iso

@mkdir -p build

build/boot.o: kernel/larch/boot.s
	@mkdir -p build
	$(AS) $(ASFLAGS) $< -o $@

build/context_switch.o: kernel/sched/context_switch.S
	@mkdir -p build
	$(AS) $(ASFLAGS) $< -o $@

build/multiboot.o: kernel/larch/multiboot.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/idt.o: kernel/larch/idt.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/gdt_full.o: kernel/gdt/gdt_full.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/kernel_main.o: kernel/kernel_main.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/pmm.o: kernel/mem/pmm.rs
	@mkdir -p build
	$(RUSTC) $(RUSTFLAGS) --emit obj $< -o $@

build/vmm.o: kernel/mem/vmm.rs
	@mkdir -p build
	$(RUSTC) $(RUSTFLAGS) --emit obj $< -o $@

build/keyboard.o: kernel/drivers/keyboard.rs
	@mkdir -p build
	$(RUSTC) $(RUSTFLAGS) --emit obj $< -o $@

build/pit.o: kernel/drivers/pit.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/vga_scroll.o: kernel/drivers/vga_scroll.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/vga_cursor.o: kernel/drivers/vga_cursor.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/rtc.o: kernel/drivers/rtc.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/ps2_mouse.o: kernel/drivers/ps2_mouse.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/serial.o: kernel/drivers/serial.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/kprintf.o: kernel/printf/kprintf.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/vfs.o: kernel/fs/vfs.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/initramfs.o: initramfs.cpio
	@mkdir -p build
	$(LD) -r -b binary -o $@ $<

build/elf_loader.o: kernel/userspace/elf_loader.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/syscall_handler.o: kernel/syscall/syscall_handler.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/syscall_implementations.o: kernel/syscall/syscall_implementations.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/shell_main.o: kernel/shell/shell_main.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/builtin_commands.o: kernel/shell/builtin_commands.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/cmd_cat.o: kernel/shell/cmd_cat.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/help_updated.o: kernel/shell/help_updated.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/command_history.o: kernel/shell/command_history.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/colored_shell.o: kernel/shell/colored_shell.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

larch.bin: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

larch.iso: larch.bin
	mkdir -p iso/boot/grub
	cp larch.bin iso/boot/
	@printf 'set timeout=0\nset default=0\n\nmenuentry "Larch OS" {\n    multiboot2 /boot/larch.bin\n    boot\n}\n' > iso/boot/grub/grub.cfg
	grub-mkrescue -o larch.iso iso/
	@echo ""
	@echo "larch.iso built successfully!"
	@echo "  Test with: qemu-system-i386 -cdrom larch.iso -m 256M"

initramfs.cpio:
	@echo "Building initramfs..."
	mkdir -p initramfs/bin
	echo '#!/bin/sh'                   > initramfs/bin/hello
	echo 'echo "Hello from Larch!"'   >> initramfs/bin/hello
	chmod +x initramfs/bin/hello
	cd initramfs && find . | cpio -o -H newc > ../initramfs.cpio
	@echo "initramfs.cpio created"

run: larch.iso
	qemu-system-i386 -cdrom larch.iso -m 256M

run-debug: larch.iso
	qemu-system-i386 -cdrom larch.iso -m 256M -serial stdio -d int,cpu_reset

clean:
	rm -rf build/ larch.bin larch.iso iso/
	@echo " Cleaned"

clean-all: clean
	rm -f initramfs.cpio
	rm -rf initramfs/

info:
	@echo "Larch OS Build Info"
	@echo "  CC:      $(CC)"
	@echo "  RUSTC:   $(RUSTC)"
	@echo "  Objects: $(words $(OBJS)) files"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build larch.iso"
	@echo "  make run      - Build + run in QEMU"
	@echo "  make run-debug - Run with serial output + interrupt log"
	@echo "  make clean    - Remove build artifacts"
	@echo "  make clean-all - Remove everything including initramfs"

.PHONY: all clean clean-all run run-debug info