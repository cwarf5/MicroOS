# Compiler/Assembler/Emulator
CC = gcc
ASM = nasm
LD = ld
QEMU = qemu-system-i386

# Directories
BOOT_DIR = bootloader
KERNEL_DIR = kernel

# Source files
BOOT_SOURCE = $(BOOT_DIR)/boot.asm
KERNEL_ENTRY = $(KERNEL_DIR)/kernel_entry.asm
KERNEL_ASM = $(KERNEL_DIR)/keyboard_handler.asm
KERNEL_SOURCE = $(KERNEL_DIR)/kernel_shell.c $(KERNEL_DIR)/interrupts.c

# Object files
BOOT_BIN = boot.bin
KERNEL_ENTRY_OBJ = kernel_entry.o
KERNEL_ASM_OBJ = $(KERNEL_ASM:.asm=.o)
KERNEL_OBJS = $(addprefix $(KERNEL_DIR)/, $(notdir $(KERNEL_SOURCE:.c=.o)))
KERNEL_BIN = kernel.bin

# Output
OS_IMAGE = os-image
DISK_IMG = disk.img

# Flags
CFLAGS = -m32 -c -ffreestanding -fno-pie -fno-stack-protector -nostdlib -Wall
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T $(KERNEL_DIR)/linker.ld --oformat binary -nostdlib

# Default target
all: $(OS_IMAGE)

# Compile each kernel C source file individually to its corresponding .o file
$(KERNEL_DIR)/%.o: $(KERNEL_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

# Assemble boot sector
$(BOOT_BIN): $(BOOT_SOURCE)
	$(ASM) -f bin $< -o $@

# Assemble kernel entry
$(KERNEL_ENTRY_OBJ): $(KERNEL_ENTRY)
	$(ASM) $(ASMFLAGS) $< -o $@

# Assemble kernel assembly files
$(KERNEL_ASM_OBJ): $(KERNEL_ASM)
	$(ASM) $(ASMFLAGS) $< -o $@

# Link kernel
$(KERNEL_BIN): $(KERNEL_ENTRY_OBJ) $(KERNEL_ASM_OBJ) $(KERNEL_OBJS)
	$(LD) $(LDFLAGS) $^ -o $@

# Create OS image
$(OS_IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $^ > $@

# Create bootable disk image
$(DISK_IMG): $(OS_IMAGE)
	dd if=/dev/zero of=$@ bs=512 count=2880
	dd if=$(BOOT_BIN) of=$@ bs=512 count=1 conv=notrunc
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=1 conv=notrunc

# Run in QEMU
run: $(DISK_IMG)
	$(QEMU) -drive format=raw,file=$<,if=floppy

# Debug with QEMU and GDB
debug: $(DISK_IMG)
	$(QEMU) -drive format=raw,file=$<,if=floppy -s -S

# Clean build files
clean:
	rm -f *.bin *.o $(OS_IMAGE) $(DISK_IMG) qemu.log $(KERNEL_DIR)/*.o

.PHONY: all run debug clean